#include "inttypes.h" // now types like uint32_t can be used.

typedef enum { RAMP_LIN, RAMP_EXP } RampTypes_t;

class MotionThread
{
public:

  bool configure(int speed_sps, float accFactor, RampTypes_t rampType);

  bool start(bool turn, int steps, int speed_sps, float accFactor);

  bool startCircle(int32_t stepsLeft, int32_t stepsRight, int32_t speedLeft, int32_t speedRight);

  static void stop();

  static bool isMoving() { return moving; };


private:

  static void pulse();

  static void pulseCircle();
  
  static unsigned long int rampFunc(unsigned long int speed, unsigned long int target);


  static bool moving;
  
  static int step;
  
  static bool abortMotion;

  static float acc;

  static unsigned long int stepInterval;

  static unsigned long int currentSpeed_sps;
  
  static unsigned long int targetSpeed_sps;

  static unsigned long int  currentSpeedLeft_sps;
  
  static unsigned long int  currentSpeedRight_sps;
  
  static unsigned long int  targetSpeedLeft_sps;
  
  static unsigned long int  targetSpeedRight_sps;

  static unsigned long int  intervalRight;

  static unsigned long int  intervalLeft;

  static unsigned int       stepsForRamp;

  static RampTypes_t        ramp;

};


