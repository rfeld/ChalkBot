// Wheel, Car and Stepper dimensions
const int32_t WHEEL_DIAMETER = 103; // [mm]
const int32_t WHEEL_DISTANCE = 247; // [mm] distance between the wheels
const int32_t STEPS_PER_ROTATION = 200; // full circle of one wheel
const int32_t STEPS_PER_M = 1000 * STEPS_PER_ROTATION / ( WHEEL_DIAMETER * PI );
const int32_t STEPS_PER_TURN =  STEPS_PER_M * (WHEEL_DISTANCE * PI ) / 1000;



// GPIO definitions
#define EN        8       //The step motor makes the power end, low level is effective
#define X_DIR     5       //The x-axis moves in the direction of the motor
#define Y_DIR     6       //The y-axis moves in the direction of the motor
#define X_STP     2       //The x axis stepper control
#define Y_STP     3       //The y axis stepper control


