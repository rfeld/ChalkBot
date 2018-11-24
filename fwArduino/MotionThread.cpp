#ifndef MOTIONTHREAD_H
#define MOTIONTHREAD_H

#include "MotionThread.h"
#include <uTimerLib.h>
#include "definitions.h"
#include <SoftwareSerial.h>

bool MotionThread::abortMotion = false;
int  MotionThread::step = 0;
bool MotionThread::moving = false;
float MotionThread::acc = 1.1;
unsigned long int  MotionThread::stepInterval = 10000;
unsigned long int  MotionThread::currentSpeed_sps = 10;
unsigned long int  MotionThread::targetSpeed_sps  = 100;



/////////////////////////////////////////////////////////
// start
/////////////////////////////////////////////////////////
void MotionThread::start(int steps, bool dir, int speed_sps, float accFactor)
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

  currentSpeed_sps = 10;
  targetSpeed_sps  = speed_sps;

  acc = accFactor;
  
  stepInterval = 1000000/currentSpeed_sps;

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

  stepInterval = rampUp();

  step--;
  if(abortMotion || step<=0) stop();
  else      TimerLib.setTimeout_us(pulseStart, (stepInterval-pulseWidth) );
  
}


/////////////////////////////////////////////////////////
// rampUp
/////////////////////////////////////////////////////////
static unsigned long int MotionThread::rampUp()
{ 
  if(currentSpeed_sps<targetSpeed_sps)
  {
    currentSpeed_sps = currentSpeed_sps * acc;
  }

  if(currentSpeed_sps>targetSpeed_sps) currentSpeed_sps = targetSpeed_sps;

  return (1000000/currentSpeed_sps);
}


#endif // MOTIONTHREAD_H

