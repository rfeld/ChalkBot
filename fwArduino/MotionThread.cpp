#ifndef MOTIONTHREAD_H
#define MOTIONTHREAD_H

#include "MotionThread.h"
#include <uTimerLib.h>
#include "definitions.h"

bool MotionThread::abortMotion = false;
int  MotionThread::step = 0;
bool MotionThread::moving = false;
int  MotionThread::stepInterval = 10000;


/////////////////////////////////////////////////////////
// start
/////////////////////////////////////////////////////////
void MotionThread::start(int steps, bool dir, int speed_sps)
{
  moving = true;
  abortMotion  = false;

  if(dir)
  {    
    digitalWrite(X_DIR, LOW);
    digitalWrite(Y_DIR, LOW);
  }
  else
  {
    digitalWrite(X_DIR, HIGH);
    digitalWrite(Y_DIR, HIGH);
  }
  
  digitalWrite(EN, LOW);

  step = steps;
  stepInterval = 1000000/speed_sps;

  pulseStart(); 
}


/////////////////////////////////////////////////////////
// stop
/////////////////////////////////////////////////////////
static void MotionThread::stop()
{
  digitalWrite(EN, HIGH);
  abortMotion = true;  
  moving = false;
  step = 0;
}


/////////////////////////////////////////////////////////
// pulseStart
/////////////////////////////////////////////////////////
static void MotionThread::pulseStart()
{
  digitalWrite(X_STP, LOW);
  digitalWrite(Y_STP, LOW);
  if(abortMotion) stop();
  else            TimerLib.setTimeout_us(pulseEnd, pulseWidth);  
}


/////////////////////////////////////////////////////////
// pulseEnd
/////////////////////////////////////////////////////////
static void MotionThread::pulseEnd() 
{      
  digitalWrite(X_STP, HIGH);
  digitalWrite(Y_STP, HIGH);

  step--;
  if(abortMotion || step<=0) stop();
  else      TimerLib.setTimeout_us(pulseStart, (stepInterval-pulseWidth) );
  
}


#endif // MOTIONTHREAD_H

