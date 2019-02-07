#include "inttypes.h" // now types like uint32_t can be used.


typedef enum { BOTH, RIGHT, LEFT } Wheel_t;

class MotionThread
{
public:

  bool start(bool turn, int steps, int speed_sps, float accFactor);

  bool startCircle(int32_t stepsLeft, int32_t stepsRight, int32_t speedLeft, int32_t speedRight);

  static void stop();

  static bool isMoving() { return moving; };


private:

  static void pulse();

  static void pulseCircle();

  static Wheel_t whichWheel();
  
  static unsigned long int rampUp_exp();

  static unsigned long int rampUp_lin();

  
  static Wheel_t wheel;

  static bool moving;
  
  static int step;
  
  static bool abortMotion;

  static float acc;

  static unsigned long int stepInterval;

  static unsigned long int currentSpeed_sps;
  
  static unsigned long int targetSpeed_sps;

  static unsigned long int  currentSpeedLeft_sps;
  
  static unsigned long int  currentSpeedRight_sps;
  
  static unsigned long int  baseSpeedLeft_sps;
  
  static unsigned long int  baseSpeedRight_sps;

  static unsigned long int  intervalRight;

  static unsigned long int  intervalLeft;

};


