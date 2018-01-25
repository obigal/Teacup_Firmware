/*
	This file contains defines that change the way the step/dir pins work, such
	that they become interchangable graycode signaling pins. If your stepper 
	driver expects coil polarity signals instead of step/dir, this will allow
	them to work with reprap.

	By default, X Y and Z are graycode, while E remains step/dir. If this isn't what
	you want, edit the lines just below.
*/

#include  <stdint.h>

#define X_GREYCODE
#define Y_GREYCODE
#define Z_GREYCODE
#define E_GREYCODE

/*
	X Stepper **ULN2003_DRIVER**
*/
#ifdef X_GREYCODE
	#undef x_step
	#undef _x_step
	#undef x_direction
	#define x_step() {\
        x_greycode+=stored_x_direction;\
        WRITE(X_STEP_1,(x_greycode+2>>1)&1);\
        WRITE(X_STEP_2,((x_greycode>>1)^x_greycode)&1);\
        WRITE(X_STEP_3,(x_greycode>>1)&1);\
        WRITE(X_STEP_4,((x_greycode+2>>1)^x_greycode)&1);\
        }
	#define _x_step(st)                    
	#define x_direction(dir)        stored_x_direction=(dir)?1:-1; x_enabled=1; 
	#define unstep_x() \
        do { \
        if (x_enabled) { WRITE(X_STEP_1,0); WRITE(X_STEP_2,0);\
        WRITE(X_STEP_3,0); WRITE(X_STEP_4,0); x_enabled=0; }\
        } while (0)
	int8_t stored_x_direction;
	int8_t x_greycode;
	uint8_t x_enabled;
#endif

/*
	Y Stepper
*/
#ifdef Y_GREYCODE
	#undef y_step
	#undef _y_step
	#undef y_direction
	#define y_step() {\
        y_greycode+=stored_y_direction;\
        WRITE(Y_STEP_1,(y_greycode+2>>1)&1);\
        WRITE(Y_STEP_2,((y_greycode>>1)^y_greycode)&1);\
        WRITE(Y_STEP_3,(y_greycode>>1)&1);\
        WRITE(Y_STEP_4,((y_greycode+2>>1)^y_greycode)&1);\
        }
	#define _y_step(st)                     
	#define y_direction(dir)        stored_y_direction=(dir)?1:-1; y_enabled=1;
	#define unstep_y() \
        do { \
        if (y_enabled) { WRITE(Y_STEP_1,0); WRITE(Y_STEP_2,0);\
        WRITE(Y_STEP_3,0); WRITE(Y_STEP_4,0); y_enabled=0; }\
        } while (0)
	int8_t stored_y_direction;
	int8_t y_greycode;
	uint8_t y_enabled;
#endif

/*
	Z Stepper
*/
#ifdef Z_GREYCODE
	#undef z_step
	#undef _z_step
	#undef z_direction
	#define z_step() {\
        z_greycode+=stored_z_direction;\
        WRITE(Z_STEP_1,(z_greycode+2>>1)&1);\
        WRITE(Z_STEP_2,((z_greycode>>1)^z_greycode)&1);\
        WRITE(Z_STEP_3,(z_greycode>>1)&1);\
        WRITE(Z_STEP_4,((z_greycode+2>>1)^z_greycode)&1);\
        }
	#define _z_step(st)                    
	#define z_direction(dir)        stored_z_direction=(dir)?1:-1; z_enabled=1;
	#define unstep_z() \
        do { \
        if (z_enabled) { WRITE(Z_STEP_1,0); WRITE(Z_STEP_2,0);\
        WRITE(Z_STEP_3,0); WRITE(Z_STEP_4,0); z_enabled=0; }\
        } while (0)
	int8_t stored_z_direction;
	int8_t z_greycode;
	uint8_t z_enabled;
#endif

/*
	Extruder
*/
#ifdef E_GREYCODE
	#undef e_step
	#undef _e_step
	#undef e_direction
	#define e_step() {\
        e_greycode+=stored_e_direction;\
        WRITE(E_STEP_1,(e_greycode+2>>1)&1);\
        WRITE(E_STEP_2,((e_greycode>>1)^e_greycode)&1);\
        WRITE(E_STEP_3,(e_greycode>>1)&1);\
        WRITE(E_STEP_4,((e_greycode+2>>1)^e_greycode)&1);\
        }
	#define _e_step(st)                     
	#define e_direction(dir)        stored_e_direction=(dir)?1:-1; e_enabled=1;
	#define unstep_e() \
        do { \
        if (e_enabled) { WRITE(E_STEP_1,0); WRITE(E_STEP_2,0);\
        WRITE(E_STEP_3,0); WRITE(E_STEP_4,0); e_enabled=0; }\
        } while (0)
	int8_t stored_e_direction;
	int8_t e_greycode;
	uint8_t e_enabled;
#endif
