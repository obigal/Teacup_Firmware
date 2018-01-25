/*
  motion_control.c - high level interface for issuing motion commands
  Part of Grbl

  Copyright (c) 2009-2011 Simen Svale Skogsrud
  Copyright (c) 2011 Sungeun K. Jeon

  Grbl is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Grbl is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with Grbl.  If not, see <http://www.gnu.org/licenses/>.
*/

#include  <string.h>
#include  <stdlib.h>
#include  <math.h>
#include  <avr/interrupt.h>

#include  "dda.h"
#include  "dda_queue.h"
#include  "dda_maths.h"
#include  "gcode_parse.h"

// Arc interpretation settings:
#ifndef MM_PER_ARC_SEGMENT
  #define MM_PER_ARC_SEGMENT 1
  #warning MM_PER_ARC_SEGMENT not user defined, defaulting to 1.
#endif
// Number of arc generation iterations by small angle approximation before exact arc trajectory
// correction. This parameter maybe decreased if there are issues with the accuracy of the arc
// generations. In general, the default value is more than enough for the intended CNC applications
// of grbl, and should be on the order or greater than the size of the buffer to help with the
// computational efficiency of generating arcs.
#ifndef N_ARC_CORRECTION
  #define N_ARC_CORRECTION 25 // Integer (1-255)
  #warning N_ARC_CORRECTION not user defined, defaulting to 25.
#endif

// The arc is approximated by generating a huge number of tiny, linear segments. The length of each
// segment is configured in settings.mm_per_arc_segment.
void mc_arc(uint32_t radius, uint8_t isclockwise)
{
  int32_t center_axis0 = startpoint.axis[X] + next_target.I;                  //center x
  int32_t center_axis1 = startpoint.axis[Y] + next_target.J;                  //center y
  int32_t linear_travel = next_target.target.axis[Z] - startpoint.axis[Z];    //helical
  int32_t extruder_travel = next_target.target.axis[E] - startpoint.axis[E];  //extruder

  float r_axis0 = -next_target.I / 1000.0;
  float r_axis1 = -next_target.J / 1000.0;
  float rt_axis0 = (next_target.target.axis[X] - center_axis0) / 1000.0;
  float rt_axis1 = (next_target.target.axis[Y] - center_axis1) / 1000.0;

  // CCW angle between position and target from circle center. Only one atan2() trig computation required.
  float angular_travel = atan2(r_axis0 * rt_axis1 - r_axis1 * rt_axis0, r_axis0 * rt_axis0 + r_axis1 * rt_axis1);
  if (angular_travel < 0) {
    angular_travel += 2 * M_PI;   //<0: -3141..0 --> 3141..6282 ; 0..6282
  }
  if (isclockwise) {
    angular_travel -= 2 * M_PI;   // -6282..0
  }

  //the next integer approx will overflow if the radius is very large combined with a large angular travel.
  //example: maximum angular travel =2xPI then overflow at radius>=683mm.
  //the radius was already aproximated, at a radius of 250mm the aprox already differs 8mm (258mm) about 3%
  //meaning millimeters_of_travel (actually micrometers here) will be off by some % too.
  //This only has effect for calculating the number of segments and for this the approx is accurate enough
  uint32_t millimeters_of_travel = approx_distance(labs((uint32_t)(angular_travel * radius)), labs(linear_travel));
  //if (millimeters_of_travel < 0.001) { return; }
  //if (millimeters_of_travel < 1) { return; }  //todo: just enqueue to target instead.. maybe already if <10??
  if (millimeters_of_travel < 10) {             // 0,01 mm , just go there and return
    if (millimeters_of_travel > 0)
      enqueue(&next_target.target);
    return;
  }	// integer value = 0 then. todo: just enqueue to target if <10??

  //todo: comment on this
  uint16_t mm_per_arc_segment;
  if (radius > 5000) mm_per_arc_segment = 1000 + radius / 50;
  else mm_per_arc_segment = radius / 5;
  uint16_t segments = millimeters_of_travel / mm_per_arc_segment;
  if (segments == 0) segments = 1;
  float theta_per_segment = angular_travel / segments;
  int32_t linear_per_segment = linear_travel / segments;
  int32_t extruder_per_segment = extruder_travel / segments;

  /* Vector rotation by transformation matrix: r is the original vector, r_T is the rotated vector,
     and phi is the angle of rotation. Based on the solution approach by Jens Geisler.
         r_T = [cos(phi) -sin(phi);
                sin(phi)  cos(phi] * r ;

     For arc generation, the center of the circle is the axis of rotation and the radius vector is
     defined from the circle center to the initial position. Each line segment is formed by successive
     vector rotations. This requires only two cos() and sin() computations to form the rotation
     matrix for the duration of the entire arc. Error may accumulate from numerical round-off, since
     all double numbers are single precision on the Arduino. (True double precision will not have
     round off issues for CNC applications.) Single precision error can accumulate to be greater than
     tool precision in some cases. Therefore, arc path correction is implemented.

     Small angle approximation may be used to reduce computation overhead further. This approximation
     holds for everything, but very small circles and large mm_per_arc_segment values. In other words,
     theta_per_segment would need to be greater than 0.1 rad and N_ARC_CORRECTION would need to be large
     to cause an appreciable drift error. N_ARC_CORRECTION~=25 is more than small enough to correct for
     numerical drift error. N_ARC_CORRECTION may be on the order a hundred(s) before error becomes an
     issue for CNC machines with the single precision Arduino calculations.

     This approximation also allows mc_arc to immediately insert a line segment into the planner
     without the initial overhead of computing cos() or sin(). By the time the arc needs to be applied
     a correction, the planner should have caught up to the lag caused by the initial mc_arc overhead.
     This is important when there are successive arc motions.
  */
  // Vector rotation matrix values
  float cos_T = 1 - 0.5 * theta_per_segment * theta_per_segment; // Small angle approximation
  float sin_T = theta_per_segment;
  float sin_Ti;
  float cos_Ti;
  float r_axisi;
  uint16_t i;
  int8_t count = 0;
  TARGET	arc_target;		///< target position: X, Y, Z, E and F for next arc part

  // Initialize the linear axis
  arc_target.axis[Z] = startpoint.axis[Z];

  // Initialize the extruder axis
  arc_target.axis[E] = startpoint.axis[E];

  // Initialize F
  arc_target.F = next_target.target.F;

  for (i = 1; i < segments; i++) { // Increment (segments-1)

    if (count < N_ARC_CORRECTION) {
      // Apply vector rotation matrix
      r_axisi = r_axis0 * sin_T + r_axis1 * cos_T;
      r_axis0 = r_axis0 * cos_T - r_axis1 * sin_T;
      r_axis1 = r_axisi;
      count++;
    } else {
      // Arc correction to radius vector. Computed only every N_ARC_CORRECTION increments.
      // Compute exact location by applying transformation matrix from initial radius vector(=-offset).
      cos_Ti = cos(i * theta_per_segment);
      sin_Ti = sin(i * theta_per_segment);
      r_axis0 = -(next_target.I / 1000.0) * cos_Ti + (next_target.J / 1000.0) * sin_Ti;
      r_axis1 = -(next_target.I / 1000.0) * sin_Ti - (next_target.J / 1000.0) * cos_Ti;
      count = 0;
    }

    // Update arc_target location
    arc_target.axis[X] = center_axis0 + (int32_t)(r_axis0 * 1000.0);
    arc_target.axis[Y] = center_axis1 + (int32_t)(r_axis1 * 1000.0);
    arc_target.axis[Z] += linear_per_segment;
    arc_target.axis[E] += extruder_per_segment;

    enqueue(&arc_target);
  }
  // Ensure last segment arrives at target location.
  enqueue(&next_target.target);
}

