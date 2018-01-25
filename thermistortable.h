// default thermistor lookup table

// How many thermistor tables we have
#define NUMTABLES 1

#define THERMISTOR_EXTRUDER	0
// #define THERMISTOR_BED		1

// Thermistor lookup table, generated with --num-temps=50 and trimmed in lower temperature ranges.
// You may be able to improve the accuracy of this table in various ways.
//   1. Measure the actual resistance of the resistor. It's "nominally" 4.7K, but that's ± 5%.
//   2. Measure the actual beta of your thermistor:http://reprap.org/wiki/MeasuringThermistorBeta
//   3. Generate more table entries than you need, then trim down the ones in uninteresting ranges. (done)
// In either case you'll have to regenerate this table, which requires python, which is difficult to install on windows.
// Since you'll have to do some testing to determine the correct temperature for your application anyway, you
// may decide that the effort isn't worth it. Who cares if it's reporting the "right" temperature as long as it's
// keeping the temperature steady enough to print, right?
// ./createTemperatureLookup.py --r0=100000 --t0=25 --r1=0 --r2=20000 --beta=4300 --max-adc=1023 --min-adc=1 --multiplier=4 --vadc=5.0 --num-temps=50
// r0: 100000
// t0: 25
// r1: 0 (parallel with rTherm)
// r2: 20000 (series with rTherm)
// beta: 4300
// min adc: 1 at 0.0048828125 V
// max adc: 1023 at 4.9951171875 V
// ADC counts from 1 to 1023 by 20
#define NUMTEMPS  20

#ifdef THERMISTOR_PULLUP
const uint16_t PROGMEM temptable[NUMTABLES][NUMTEMPS][2] = {
  {
    // {ADC, temp*4.0 }, // temp         Rtherm     Vtherm      resolution   power
    {   1,   1831}, //  457.85 C,       20 Ohm, 0.005 V, 77.15 C/count, 0.00mW
    {   6,   1147}, //  286.82 C,      118 Ohm, 0.029 V, 13.69 C/count, 0.01mW
    {  11,    982}, //  245.54 C,      217 Ohm, 0.054 V, 6.10 C/count, 0.01mW
    {  16,    891}, //  222.83 C,      317 Ohm, 0.078 V, 3.78 C/count, 0.02mW
    {  21,    829}, //  207.48 C,      419 Ohm, 0.103 V, 2.69 C/count, 0.03mW
    {  26,    784}, //  196.02 C,      521 Ohm, 0.127 V, 2.07 C/count, 0.03mW
    {  31,    747}, //  186.94 C,      624 Ohm, 0.151 V, 1.67 C/count, 0.04mW
    {  36,    717}, //  179.45 C,      729 Ohm, 0.176 V, 1.39 C/count, 0.04mW
    {  41,    692}, //  173.10 C,      834 Ohm, 0.200 V, 1.19 C/count, 0.05mW
    {  51,    651}, //  162.77 C,     1048 Ohm, 0.249 V, 0.92 C/count, 0.06mW
    {  61,    618}, //  154.56 C,     1267 Ohm, 0.298 V, 0.75 C/count, 0.07mW
    {  71,    591}, //  147.76 C,     1490 Ohm, 0.347 V, 0.63 C/count, 0.08mW
    {  81,    567}, //  141.98 C,     1718 Ohm, 0.396 V, 0.54 C/count, 0.09mW
    { 136,    480}, //  120.03 C,     3063 Ohm, 0.664 V, 0.31 C/count, 0.14mW
    { 221,    400}, //  100.03 C,     5504 Ohm, 1.079 V, 0.19 C/count, 0.21mW
    { 436,    281}, //   70.47 C,    14830 Ohm, 2.129 V, 0.11 C/count, 0.31mW
    { 631,    201}, //   50.49 C,    32112 Ohm, 3.081 V, 0.10 C/count, 0.30mW
    { 816,    120}, //   30.10 C,    78462 Ohm, 3.984 V, 0.13 C/count, 0.20mW
    { 881,     82}, //   20.75 C,   123217 Ohm, 4.302 V, 0.16 C/count, 0.15mW
    { 972,      0}  //    0.02 C,   373846 Ohm, 4.746 V, 0.35 C/count, 0.06mW
  }
};
#else
// ./createTemperatureLookup.py --r0=100000 --t0=25 --r1=0 --r2=4700 --beta=4066 --max-adc=1023
const uint16_t PROGMEM temptable[NUMTABLES][NUMTEMPS][2] = {
  {
    {1, 3364}, // 841.027617469 C
    {21, 1329}, // 332.486789769 C
    {41, 1104}, // 276.102666373 C
    {61, 987}, // 246.756060004 C
    {81, 909}, // 227.268080588 C
    {101, 851}, // 212.78847342 C
    {121, 805}, // 201.30176775 C
    {141, 767}, // 191.787692666 C
    {161, 734}, // 183.662212795 C
    {181, 706}, // 176.561442671 C
    {201, 680}, // 170.244089549 C
    {221, 658}, // 164.542298163 C
    {241, 637}, // 159.33475843 C
    {321, 567}, // 141.921298995 C
    {381, 524}, // 131.166509425 C
    {581, 406}, // 101.561865389 C
    {781, 291}, // 72.9710018071 C
    {881, 219}, // 54.8051659223 C
    {981, 93}, // 23.4825243529 C
    {1010, 1} // 0.498606463441 C
  }
};
#endif
