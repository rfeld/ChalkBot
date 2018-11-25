/**
 * Test branch für den Chalkbot der ihn rechtsrum - linksrum fahren lässt
 * 
 * \author Birgit Feld
 * \date 2018
 */


#include "definitions.h"
#include <SoftwareSerial.h>
#include <uTimerLib.h>
#include "speedController.h"
#include "MotionThread.h"



SoftwareSerial UartToESP(11,10); // Rx, Tx
String inputString = "";

bool startStop = false;
int dir = true;
int speed_sps = 100;
float accFactor = 1.2;

MotionThread motionThread;

/**
 * setup
 */
void setup()
{
  // set up the IO tube feet used by the motor to be set to output
  pinMode(X_DIR, OUTPUT); pinMode(X_STP, OUTPUT);
  pinMode(Y_DIR, OUTPUT); pinMode(Y_STP, OUTPUT);
  digitalWrite(X_DIR, LOW);
  digitalWrite(Y_DIR, LOW);
  pinMode(EN, OUTPUT);

  // Prepare to use the LED
  pinMode(13,OUTPUT);
  
  digitalWrite(EN, HIGH);
  
  Serial.begin(115200);
  UartToESP.begin(9600);

  UartToESP.println("started");

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
        // split string at ":" to get command
        int endOfCmd = inputString.indexOf(":");
        String cmd = inputString.substring(0,endOfCmd); 
        cmd.trim();
        
        if(cmd=="start") 
        {
          startStop = true;
        }
        else if(cmd=="v") 
        {
          int newSpeed = inputString.substring(endOfCmd+1,-1).toInt();
          if(newSpeed > 0 && newSpeed < 250) speed_sps = newSpeed;
          Serial.print("speed: ");
          Serial.println(speed_sps);
        }
        else if(cmd=="a") 
        {
          float newAcc = inputString.substring(endOfCmd+1,-1).toFloat();
          if(newAcc >= 1.1) accFactor = newAcc;
          Serial.print("acc: ");
          Serial.println(accFactor);
        }
        else
        {
          startStop = false;
        }
                                    
        // clear for new input
        inputString = "";
      }
  }  
}


void testMove()
{

  // sobald die eine Bewegung beendet ist, wird die Richtung gewechselt
  // und wieder ine Bewegung in die andere Richtung gestartet. Immer je 
  // eine Umdrehung (200 Schritte)
  if(!motionThread.isMoving() && startStop)
  {
    UartToESP.println("toggle direction");
    dir = !dir;
    motionThread.start(200, dir, speed_sps, accFactor);
  }
  if(!startStop)
  {
    motionThread.stop();
  }
}



/**
 * main loop
 */
void loop()
{  
  // check for data from ESP/WiFi
  if (UartToESP.available() > 0) processInput();

  testMove();
  
}




