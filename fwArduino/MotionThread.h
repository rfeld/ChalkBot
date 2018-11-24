

class MotionThread
{
public:

  void start(int steps, bool dir, int speed_sps);

  static void stop();

  static bool isMoving() { return moving; };


private:

  static void pulseStart();

  static void pulseEnd();

  static bool moving;
  
  static int step;
  
  static bool abortMotion;

  static int stepInterval;

  static const int pulseWidth = 100;

};


