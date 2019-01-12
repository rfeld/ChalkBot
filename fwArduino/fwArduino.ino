/**
   FW für den Chalkbot

   \author Birgit Feld
   \date 2018
*/


#include "definitions.h"
#include <SoftwareSerial.h>
#include <uTimerLib.h>
#include "speedController.h"
#include "MotionThread.h"
#include "inttypes.h" // now types like uint32_t can be used.



SoftwareSerial UartToESP(11, 10); // Rx, Tx
String inputString = "";

// communication protocoll parameters
const unsigned int  MAX_CMDS = 5;
String              cmdQueue[MAX_CMDS];
unsigned int        cmdIndex=0;
enum ErrorCode {  
  NO_ERROR = 0,
  INVALID_CMD,
  INVALID_PARAMETER,
  CMD_QUEUE_FULL,
  SW_ERROR
};



bool startStop = false;
int dir = true;
int speed_sps = 100;
float accFactor = 1.2;


MotionThread motionThread;



/** ********************************************************
   setup
************************************************************ */
void setup()
{
  // set up the IO tube feet used by the motor to be set to output
  pinMode(X_DIR, OUTPUT); pinMode(X_STP, OUTPUT);
  pinMode(Y_DIR, OUTPUT); pinMode(Y_STP, OUTPUT);
  digitalWrite(X_DIR, LOW);
  digitalWrite(Y_DIR, LOW);
  pinMode(EN, OUTPUT);

  // Prepare to use the LED
  pinMode(13, OUTPUT);

  digitalWrite(EN, HIGH);

  Serial.begin(115200);
  UartToESP.begin(9600);

  UartToESP.println("started");

}




/** ********************************************************
   Parse the WIFI input
************************************************************ */
void processInput()
{
  // read everything currently available
  while (UartToESP.available())
  {
    // complete line received
    char inChar = (char)UartToESP.read();
    if (inChar == '\n')
    {
      // split string at ":" to get command
      int endOfCmd = inputString.indexOf(":");
      String cmd = inputString.substring(0, endOfCmd);
      cmd.trim();

      // stop
      if (cmd == "stop")
      {
        motionThread.stop();
        UartToESP.print("ACK: ");
        UartToESP.println(MAX_CMDS);
        cmdIndex = 0;
      }      
      // for command queue
      else if ( cmd == "move" || cmd == "turn" || cmd == "stepper" || cmd == "chalk" )
      {
        if (cmdIndex < MAX_CMDS)
        {
          cmdQueue[cmdIndex++] = inputString;
          UartToESP.print("ACK: ");
          UartToESP.println(MAX_CMDS - cmdIndex);
        }
        else
        {
          String answer = "ERR: ";
          answer += CMD_QUEUE_FULL;
          answer += ", cmd queue full";
          UartToESP.println( answer );
        }
      }      
      // invalid command
      else
      {
        String answer = "ERR: ";
        answer += INVALID_CMD;
        answer += ", invalid command ";
        answer += cmd;
        motionThread.stop();
        UartToESP.println(answer);
        cmdIndex = 0;
      }

      // clear for new input
      inputString = "";
    }
    else
    {
      inputString += inChar;
    }
  }
}

/** ********************************************************
 * processQueue sets the new motion parameters
 * \param input contains command and parameters
************************************************************ */
void processQueue(String input)
{
    UartToESP.println("processQueue()");
  
    if(motionThread.isMoving()) 
    {
      String answer = "ERR: ";
      answer+= SW_ERROR;
      answer+= ", trying to start motion thread, though still moving";
      UartToESP.println(answer); 
      return;
    }
  
    // split string at ":" to get command
    int endOfCmd = input.indexOf(":");
    String cmd = input.substring(0,endOfCmd); 
    cmd.trim(); 
    
    // split string at "," to get parameters    
    const int MAX_PARAMS = 4;
    String params[MAX_PARAMS];   
    int from = endOfCmd+1;
    for(int i=0; i<MAX_PARAMS; i++)
    {
      params[i] = "";
      if(from>0)
      {
        int endOfParam = input.indexOf(",", from);            
        params[i] = input.substring(from,endOfParam);           
        params[i].trim();
        from = endOfParam+1;
      }                   
    }
    
    // list of all valid commands
    if     (cmd=="move")    move(false, params[0], params[1], params[2], params[3]);
    else if(cmd=="turn")    move(true,  params[0], params[1], params[2], params[3]);
//  else if(cmd=="stepper") stepperEnable(params[0]); // tbd
//  else if(cmd=="chalk")   Serial.println("chalk");  // tbd
    else
    {
      String answer = "ERR: ";
      answer+= INVALID_CMD;
      answer+= ", trying to process invalid command in queue";
      UartToESP.println(answer); 
    }
  
}

/** ********************************************************
 *  move
************************************************************ */
void move(bool turn, String distance_mm, String speed, String acc, String rampType)
{
  UartToESP.println("move()");
  
  if(distance_mm.toInt() == 0)
  {
    UartToESP.print("ERR: ");
    UartToESP.print(INVALID_PARAMETER);
    UartToESP.println(", invalid distance");
    return;
  }


  int32_t distance = 0;

  if(turn)
  {
    distance = (distance_mm.toInt() * STEPS_PER_TURN) / 360;
    
    UartToESP.print("DEBUG: angle = ");
    UartToESP.println(distance_mm.toInt() );
    UartToESP.print("DEBUG: steps = ");
    UartToESP.println(distance);
  }
  else
  {
    distance = (distance_mm.toInt() * STEPS_PER_M) / 1000;
    
  } 

  // for all other parameters use old value if not set correctly
  if(speed.toInt() > 0 && speed.toInt() < 300)  speed_sps = speed.toInt(); 
  if(acc.toFloat()>1.1)                         accFactor = acc.toFloat();
  if(rampType == "lin" || rampType == "exp" )   ; // tbd - currently ignored

  motionThread.start(turn, distance, speed_sps, accFactor);
}




/** ********************************************************
 *  nextInQueue
************************************************************ */
void nextInQueue()
{          
  processQueue(cmdQueue[0]);
  
  // move elements in queue
  for(int i=0; i<cmdIndex; i++) cmdQueue[i]=cmdQueue[i+1];
  cmdIndex--;

  UartToESP.print("DONE: ");
  UartToESP.println(MAX_CMDS-cmdIndex);
}





/** ********************************************************
   main loop
************************************************************ */
void loop()
{
  // check for data from ESP/WiFi
  if (UartToESP.available() > 0) processInput();

  if(cmdIndex>0 && !motionThread.isMoving()) nextInQueue();

}




