// Wheel, Car and Stepper dimensions
const long WHEEL_DIAMETER = 103; // [mm]
const long WHEEL_DISTANCE = 247; // [mm] distance between the wheels
const long STEPS_PER_ROTATION = 200; // full circle of one wheel
const long STEPS_PER_M = 1000 * STEPS_PER_ROTATION / ( WHEEL_DIAMETER * 3.1415 );
const long STEPS_PER_TURN =  STEPS_PER_M * (WHEEL_DISTANCE * 3.1415 ) / 1000;

// GPIO definitions
#define EN        8       //The step motor makes the power end, low level is effective
#define X_DIR     5       //The x-axis moves in the direction of the motor
#define Y_DIR     6       //The y-axis moves in the direction of the motor
#define X_STP     2       //The x axis stepper control
#define Y_STP     3       //The y axis stepper control
