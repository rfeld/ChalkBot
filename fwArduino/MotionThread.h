

class MotionThread
{
public:

  void start(int steps, bool dir, int speed_sps, float accFactor);

  static void stop();

  static bool isMoving() { return moving; };


private:

  static void pulseStart();

  static void pulseEnd();

  static unsigned long int rampUp_exp();

  static unsigned long int rampUp_lin();

  

  static bool moving;
  
  static int step;
  
  static bool abortMotion;

  static const int pulseWidth = 100;

  static float acc;

  static unsigned long int stepInterval;

  static unsigned long int currentSpeed_sps;
  
  static unsigned long int targetSpeed_sps;

};


