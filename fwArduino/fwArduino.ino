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
bool enableDebug = false;

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
float accFactor = 6; // 6 is a good value for linear ramp. (1.5 for exp)
RampTypes_t ramp = RAMP_LIN;



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
         
      else if(cmd=="debugOn")  enableDebug = true;
      else if(cmd=="debugOff") enableDebug = false;
      else if(cmd=="")         DebugMsg("empty line -> ignore");
      
      // for command queue
      else if ( cmd == "move"    || 
                cmd == "turn"    || 
                cmd == "stepper" || 
                cmd == "chalk"   || 
                cmd == "circle"  || 
                cmd == "config"   )  
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
    DebugMsg("processQueue()");
  
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
    else if(cmd=="circle")  circle(params[0], params[1]);
    else if(cmd=="config")  configure( params[0], params[1], params[2]);
    else
    {
      String answer = "ERR: ";
      answer+= INVALID_CMD;
      answer+= ", trying to process invalid command in queue";
      UartToESP.println(answer); 
    }
  
}

/** ********************************************************
 *  configure use old value or default if not set correctly
 *  \param speed
 *  \param acc accelaration factor (depending on the ramp type)
 *  \param rampType
************************************************************ */
void configure(String speed, String acc, String rampType)
{
  
  if(speed.toInt() > 0 && speed.toInt() < 300)  speed_sps = speed.toInt(); 
 
  if(acc.toFloat()>1.1)                         accFactor = acc.toFloat();
    
  if      (rampType == "exp") ramp = RAMP_EXP;
  else if (rampType == "lin") ramp = RAMP_LIN;

  motionThread.configure(speed_sps, accFactor, ramp);
}

/** ********************************************************
 *  move
 *  \param turn true for turns, false for linear moves
 *  \param dist distance in mm for linear moves (turn=false)
 *              or
 *              tenth of degree for rotations (turn=true)
 *  \param acc accelaration factor (depending on the ramp type)
************************************************************ */
void move(bool turn, String dist, String speed, String acc, String rampType)
{
  DebugMsg("move()");
  
  if(dist.toInt() == 0)
  {
    UartToESP.print("ERR: ");
    UartToESP.print(INVALID_PARAMETER);
    UartToESP.println(", invalid distance");
    return;
  }


  int32_t distance = 0;

  if(turn)
  {
    distance = (dist.toInt() * STEPS_PER_TURN) / 3600;   
  }
  else
  {
    distance = (dist.toInt() * STEPS_PER_M) / 1000;  
  } 

  configure(speed, acc, rampType);
  
  bool success = motionThread.start(turn, distance, speed_sps, accFactor);
  if(!success) DebugMsg("SW error in: move()");
}



/** ********************************************************
 *  circle - draw a circle
 *  \param degree 3600 defines a full circle (negative values, change direction)
 *  \param radius in mm. Must be greater than 0
************************************************************ */
void circle(String degree, String radius)
{
  DebugMsg("circle()");
  
  if(degree.toInt() == 0 || radius.toInt() <= 0 )
  {
    UartToESP.print("ERR: ");
    UartToESP.print(INVALID_PARAMETER);
    UartToESP.println(", invalid dist or radius");
    return;
  }

  // calculate distances in mm
  int32_t distChalk_mm = 2 * PI * (radius.toInt()                    ) * degree.toInt() / 3600;
  int32_t distLeft_mm  = 2 * PI * (radius.toInt() + WHEEL_DISTANCE/2 ) * degree.toInt() / 3600;
  int32_t distRight_mm = 2 * PI * (radius.toInt() - WHEEL_DISTANCE/2 ) * degree.toInt() / 3600;  
  DebugMsg( "left: "  + String(distLeft_mm ) + " mm" );
  DebugMsg( "right: " + String(distRight_mm) + " mm" );

  // calculate wheel distances in steps
  int32_t distChalk = (distChalk_mm * STEPS_PER_M) / 1000;
  int32_t distLeft  = (distLeft_mm  * STEPS_PER_M) / 1000;
  int32_t distRight = (distRight_mm * STEPS_PER_M) / 1000;
  DebugMsg( "left: "  + String(distLeft ) );
  DebugMsg( "right: " + String(distRight) );

  // calculate wheel speeds: 
  //    The desired speed should be used for the speed of the chalk.
  //    But no wheel should be faster than 1.5x speed.  
  int32_t motionDuration_ms = abs( ( 1000 * distChalk ) / speed_sps );
  if( motionDuration_ms == 0) motionDuration_ms = 10;
  DebugMsg( "motionDuration: "  + String(motionDuration_ms ) );
  int32_t speedLeft_sps  = (distLeft  * 1000) / motionDuration_ms;
  int32_t speedRight_sps = (distRight * 1000) / motionDuration_ms;
  if( abs(speedLeft_sps) > (1.5*speed_sps) )
  {
    speedLeft_sps     = 1.5*speed_sps;
    motionDuration_ms = ( 1000 * distLeft ) / speedLeft_sps;
    speedRight_sps    = (distRight * 1000) / motionDuration_ms;
  }
  if( abs(speedRight_sps) > (1.5*speed_sps) )
  {
    speedRight_sps     = 1.5*speed_sps;
    motionDuration_ms = ( 1000 * distRight ) / speedRight_sps;
    speedLeft_sps    = (distLeft * 1000) / motionDuration_ms;
  }
  DebugMsg( "speed_sps:      "  + String(speed_sps      ) );
  DebugMsg( "speedLeft_sps:  "  + String(speedLeft_sps  ) );
  DebugMsg( "speedRight_sps: "  + String(speedRight_sps ) );

  bool success = motionThread.startCircle(distLeft, distRight, abs(speedLeft_sps), abs(speedRight_sps));
  if(!success) DebugMsg("SW error in: move()");
  
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


void DebugMsg(String msg)
{
  if(enableDebug) // disable debugging here by using if(false)
  {
    UartToESP.print("DEBUG: ");
    UartToESP.println(msg);
  }
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




