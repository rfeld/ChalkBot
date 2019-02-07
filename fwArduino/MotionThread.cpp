#ifndef MOTIONTHREAD_H
#define MOTIONTHREAD_H

#include "MotionThread.h"
#include <uTimerLib.h>
#include "definitions.h"
#include <SoftwareSerial.h>

bool MotionThread::abortMotion = false;
int  MotionThread::step = 0;
bool MotionThread::moving = false;
float MotionThread::acc = 6;
unsigned long int  MotionThread::stepInterval = 10000;
unsigned long int  MotionThread::currentSpeed_sps = 10;
unsigned long int  MotionThread::targetSpeed_sps  = 100;

unsigned long int  MotionThread::currentSpeedLeft_sps  = 10;
unsigned long int  MotionThread::currentSpeedRight_sps = 10;
unsigned long int  MotionThread::baseSpeedLeft_sps     = 10;
unsigned long int  MotionThread::baseSpeedRight_sps    = 10;
unsigned long int  MotionThread::intervalLeft = 10;
unsigned long int  MotionThread::intervalRight = 10;


/////////////////////////////////////////////////////////
// start for linear movement or turns on spot
/////////////////////////////////////////////////////////
bool MotionThread::start(bool turn, int steps, int speed_sps, float accFactor)
{
  
  if(moving) return false;
  
  moving = true;
  abortMotion  = false;

  if(steps<0 && turn)  {  digitalWrite(X_DIR, LOW);   digitalWrite(Y_DIR, LOW);  }
  if(steps>0 && turn)  {  digitalWrite(X_DIR, HIGH);  digitalWrite(Y_DIR, HIGH); }
  if(steps<0 && !turn) {  digitalWrite(X_DIR, LOW);   digitalWrite(Y_DIR, HIGH); }
  if(steps>0 && !turn) {  digitalWrite(X_DIR, HIGH);  digitalWrite(Y_DIR, LOW);  }
  // if steps = 0, don't care about direction, it is not moving anyway
  
  digitalWrite(EN, LOW);

  if(steps<0)steps=-steps;

  step = steps;

  currentSpeed_sps = 10;
  targetSpeed_sps  = speed_sps;

  acc = accFactor;
  
  stepInterval = 1000000/currentSpeed_sps;
  pulse(); 
  
  return true;
}


/////////////////////////////////////////////////////////
// start for circle movements
/////////////////////////////////////////////////////////
bool MotionThread::startCircle(int32_t stepsLeft, int32_t stepsRight, int32_t speedLeft, int32_t speedRight)
{
  if(moving) return false;  
  moving = true;
  abortMotion  = false;

  currentSpeedRight_sps = speedRight;
  baseSpeedRight_sps    = speedRight;
  currentSpeedLeft_sps  = speedLeft;
  baseSpeedLeft_sps     = speedLeft;  
  intervalRight = 1000000/currentSpeedRight_sps;
  intervalLeft  = 1000000/currentSpeedLeft_sps;
  
  if(stepsRight>0)  {  digitalWrite(X_DIR, LOW);   }
  else              {  digitalWrite(X_DIR, HIGH);  }
  if(stepsLeft>0)   {  digitalWrite(Y_DIR, HIGH);   }
  else              {  digitalWrite(Y_DIR, LOW);  }

  digitalWrite(EN, LOW);

  if(stepsRight<0)stepsRight=-stepsRight;
  if(stepsLeft <0)stepsLeft =-stepsLeft;
  step = stepsRight + stepsLeft;
//
//  Serial.println("motionThread");
//  Serial.print(speedRight);
//  Serial.print(" ");
//  Serial.println(speedLeft);
//  Serial.print(intervalRight);
//  Serial.print(" ");
//  Serial.println(intervalLeft);  
//  stop();

  pulseCircle(); 

  return true;
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
// pulse
/////////////////////////////////////////////////////////
static void MotionThread::pulse() 
{      
  digitalWrite(X_STP, HIGH);
  digitalWrite(Y_STP, HIGH);

  // delay needed?
  // tests showed no problems with this, but...

  digitalWrite(X_STP, LOW);
  digitalWrite(Y_STP, LOW);

  stepInterval = rampUp_lin();

  step--;
  if(abortMotion || step<=0) stop();
  else      TimerLib.setTimeout_us(pulse, stepInterval );
  
}


/////////////////////////////////////////////////////////
// pulseCircle
/////////////////////////////////////////////////////////
static void MotionThread::pulseCircle() 
{ 

  if( whichWheel() == RIGHT )
  {
    digitalWrite(X_STP, HIGH);
    // delay needed?
    // tests showed no problems with this, but...
    digitalWrite(X_STP, LOW);
  }
  else
  {
    digitalWrite(Y_STP, HIGH);
    // delay needed?
    // tests showed no problems with this, but...
    digitalWrite(Y_STP, LOW);
  }

  

//  Serial.print(stepInterval);
//  Serial.print(" ");
//  Serial.println(step);

  if(stepInterval<100) stepInterval=100;

  step--;
  if(abortMotion || step<=0) stop();
  else      TimerLib.setTimeout_us(pulseCircle, stepInterval );
  
}


/////////////////////////////////////////////////////////
// whichWheel
/////////////////////////////////////////////////////////
static Wheel_t MotionThread::whichWheel() 
{
//
//  Serial.print("intervalRight, intervalLeft: ");
//  Serial.print(intervalRight);
//  Serial.print(", ");
//  Serial.println(intervalLeft);
  
  if(intervalRight > intervalLeft) 
  {
    stepInterval  = intervalLeft;
    intervalRight = intervalRight - intervalLeft;
    intervalLeft  = 1000000/baseSpeedLeft_sps;

//    Serial.print("Process  - intervalRight, intervalLeft: ");
//    Serial.print(intervalRight);
//    Serial.print(", ");
//    Serial.println(intervalLeft);
  
    
    return LEFT;
  }
  else
  {
    stepInterval  = intervalRight;
    intervalLeft  = intervalLeft - intervalRight;
    intervalRight    = 1000000/baseSpeedRight_sps; 

//      Serial.print("Process  - intervalRight, intervalLeft: ");
//      Serial.print(intervalRight);
//      Serial.print(", ");
//      Serial.println(intervalLeft);
  
    return RIGHT;
  }  
}


/////////////////////////////////////////////////////////
// rampUp_exp
/////////////////////////////////////////////////////////
static unsigned long int MotionThread::rampUp_exp()
{ 
  if(currentSpeed_sps<targetSpeed_sps)
  {
    currentSpeed_sps = currentSpeed_sps * acc;
  }

  if(currentSpeed_sps>targetSpeed_sps) currentSpeed_sps = targetSpeed_sps;

  return (1000000/currentSpeed_sps);
}


/////////////////////////////////////////////////////////
// rampUp_lin
/////////////////////////////////////////////////////////
static unsigned long int MotionThread::rampUp_lin()
{ 
  if(currentSpeed_sps<targetSpeed_sps)
  {
    currentSpeed_sps = currentSpeed_sps + acc;
  }

  if(currentSpeed_sps>targetSpeed_sps) currentSpeed_sps = targetSpeed_sps;

  return (1000000/currentSpeed_sps);
}



#endif // MOTIONTHREAD_H

