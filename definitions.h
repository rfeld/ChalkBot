// Wheel, Car and Stepper dimensions
const unsigned long int WHEEL_DIAMETER = 105; // [mm]
const unsigned long int WHEEL_DISTANCE = 240; // [mm] distance between the wheels
const unsigned long int STEPS_PER_ROTATION = 200; // full circle of one wheel
const unsigned long int STEPS_PER_M = 1000 * STEPS_PER_ROTATION / ( WHEEL_DIAMETER * PI );
const unsigned long int STEPS_PER_TURN =  STEPS_PER_M * (WHEEL_DISTANCE * PI ) / 1000;


// time parameters
const unsigned int BASE_CLK_us = 5000; //  faster intervals were not working
unsigned long int ticks = 0; // every tick, the timer function is called
