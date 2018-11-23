/**
 * Test branch für den Chalkbot der ihn vorwärts - rückwärts fahren lassen soll
 * 
 * \author Birgit Feld
 * \date 2018
 */


#include "definitions.h"
#include <SoftwareSerial.h>
#include <uTimerLib.h>
#include "speedController.h"



SoftwareSerial UartToESP(11,10); // Rx, Tx
String inputString = "";



// GPIO definitions
#define EN        8       //The step motor makes the power end, low level is effective
#define X_DIR     5       //The x-axis moves in the direction of the motor
#define Y_DIR     6       //The y-axis moves in the direction of the motor
#define X_STP     2       //The x axis stepper control
#define Y_STP     3       //The y axis stepper control



bool startStop = false;
bool moving = false;
int step = 0;
bool abortMotion = true;

/**
 * setup
 */
void setup()
{
  // set up the IO tube feet used by the motor to be set to output
  pinMode(X_DIR, OUTPUT); pinMode(X_STP, OUTPUT);
  pinMode(Y_DIR, OUTPUT); pinMode(Y_STP, OUTPUT);
  pinMode(EN, OUTPUT);

  // Prepare to use the LED
  pinMode(13,OUTPUT);
  
  digitalWrite(EN, HIGH);
  
  Serial.begin(115200);
  UartToESP.begin(9600);

  Serial.println("started");
}


/**
 * Parse the WIFI input 
 */
void processInput()
{
    // read everything currently available
    while (UartToESP.available()) 
    {
      char inChar = (char)UartToESP.read();      
      if (inChar != '\n') 
      {
        inputString += inChar;
      }
      else
      {   

        Serial.println("toggle move");
                               
        // received one line: toggle startStop
        startStop = !startStop;        
        
        // clear for new input
        inputString = "";
      }
  }  
}




void motionThread_stop()
{
  digitalWrite(EN, HIGH);
  abortMotion = true;  
  moving = false;
  step = 0;
}


void motionThread_start(int steps, bool dir)
{
  moving = true;
  abortMotion  = false;

  if(dir)
  {
    digitalWrite(X_STP, LOW);
    digitalWrite(Y_STP, LOW);
  }
  else
  {
    digitalWrite(X_STP, HIGH);
    digitalWrite(Y_STP, HIGH);
  }
  
  digitalWrite(EN, LOW);

  step = steps;

  motionThread_LOW();
  
}


void motionThread_LOW()
{
  digitalWrite(X_STP, LOW);
  digitalWrite(Y_STP, LOW);
  if(abortMotion) motionThread_stop();
  else      TimerLib.setTimeout_us(motionThread_HIGH, 10000);  
}


void motionThread_HIGH() 
{      
  digitalWrite(X_STP, HIGH);
  digitalWrite(Y_STP, HIGH);

  Serial.println(step);
  
  step--;
  if(abortMotion || step<=0) motionThread_stop();
  else      TimerLib.setTimeout_us(motionThread_LOW, 10000);
  
}



/**
 * main loop
 */
void loop()
{
  
  // check for data from ESP/WiFi
  if (UartToESP.available() > 0) processInput();


  // sobald die eine Bewegung beendet ist, wird die Richtung gewechselt
  // und wieder ine Bewegung in die andere Richtung gestartet. Immer je 
  // eine Umdrehung (200 Schritte)
  int dir = true;
  if(!moving && startStop)
  {
    dir != dir;
    motionThread_start(200, dir);
  }
  if(!startStop)
  {
    motionThread_stop();
  }
  
}



