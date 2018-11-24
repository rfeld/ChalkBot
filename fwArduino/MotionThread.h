

class MotionThread
{
public:

  void start(int steps, bool dir);

  static void stop();

  static bool isMoving() { return moving; };

  static void pulseStart();

  static void pulseEnd();

static bool moving;
static int step;
static bool abortMotion;
  
};


