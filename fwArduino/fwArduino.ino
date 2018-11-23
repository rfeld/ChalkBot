/**
 * Dieser schöne Code ist dafür da den ChalkBot fahren zu lassen.
 * Kommandos werden von einem über UART angbunden ESP angenommen.
 * Erreichbar ist der Arduino unter: telnet 192.168.42.47 23
 * Das Protokoll unterstützt die Kommandos: stop, move und turn (weitere folgen)
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

speedController speedy(BASE_CLK_us);

// communication protocoll parameters
const unsigned int  MAX_CMDS = 3;
String              cmdQueue[MAX_CMDS];
unsigned int        cmdIndex=0;
enum ErrorCode {  
  NO_ERROR = 0,
  INVALID_CMD,
  INVALID_PARAMETER,
  CMD_QUEUE_FULL
};

// GPIO definitions
#define EN        8       //The step motor makes the power end, low level is effective
#define X_DIR     5       //The x-axis moves in the direction of the motor
#define Y_DIR     6       //The y-axis moves in the direction of the motor
#define X_STP     2       //The x axis stepper control
#define Y_STP     3       //The y axis stepper control





// Motor Parameters
bool xDir;
bool yDir;



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

  Serial.println(STEPS_PER_M);
  Serial.println(STEPS_PER_TURN);

 TimerLib.setTimeout_us(timed_function, BASE_CLK_us);
}


/**
 * Parse the WIFI input in order to get command and parameters.
 * Call the corresponding motion function (stop, move, turn etc)
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
             
        // check command type
        if(cmd=="stop")
        {
          stop();
          UartToESP.print("ACK: ");
          UartToESP.println(MAX_CMDS);
          cmdIndex=0;          
        }
        else if( cmd=="move" || cmd=="turn" || cmd=="stepper" || cmd=="acc" || cmd=="chalk" || cmd=="version" )
        {          
          // add to command queue
          if(cmdIndex<MAX_CMDS) 
          {
            cmdQueue[cmdIndex++] = inputString;
            UartToESP.print("ACK: ");
            UartToESP.println(MAX_CMDS-cmdIndex);
          }
          else  
          {
            String answer = "ERR: ";
            answer += CMD_QUEUE_FULL;
            answer += ", cmd queue full";
            UartToESP.println( answer );
          }
  
          if(cmdIndex==1) processCmd(inputString);
        }
        else
        {
          String answer = "ERR: ";
          answer+= INVALID_CMD;
          answer+= ", invalid command ";
          answer += cmd;
          stop();
          UartToESP.println(answer);  

          // clear queue
          
        }
       // clear for new input
        inputString = "";
      }
  }  
}



/**
 * processCmd sets the new motion parameters
 * \param input contains command and parameters
 */
void processCmd(String input)
{
    // split string at ":" to get command
    int endOfCmd = input.indexOf(":");
    String cmd = input.substring(0,endOfCmd); 
    cmd.trim(); 
    
    // split string at "," to get parameters    
    const int MAX_PARAMS = 3;
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
    if     (cmd=="stop")    stop();
    else if(cmd=="move")    move(params[0], params[1], params[2]);
    else if(cmd=="turn")    turn(params[0], params[1], params[2]);
    else if(cmd=="stepper") stepperEnable(params[0]);
    else if(cmd=="acc")     { speedy.setMaxAcceleration(params[0].toInt()); nextInQueue(); }
    else if(cmd=="chalk")   Serial.println("chalk");
    else if(cmd=="version") Serial.println("version");
    else
    {
      String answer = "ERR: ";
      answer+= INVALID_CMD;
      answer+= ", trying to process invalid command ";
      UartToESP.println(answer); 
  
      // remove from command queue
      nextInQueue();
    }
}

/**
 * stop sets the steps variable to zero. At next call of timer, the motor will be stopped.
 * 
 */
void stop()
{
  debugOut("stop");
  digitalWrite(EN, HIGH);
  speedy.stop();
}


/**
 * \param enable when enable is equal to "en" steppers are enabled. Else disabled.
 * 
 * Hint! Method stop() and invalid command inputs also disables. Move() and turn() also enable.
 */
void stepperEnable(String enable)
{
  if(enable=="en") digitalWrite(EN, LOW);
  else             digitalWrite(EN, HIGH);
  nextInQueue();
}


/**
 * base function to set the robot movement parameters, used with the next timer call \n
 * 
 * \param distance  way to go in millimeter.
 *                  distance must be convertable to int. If it is not convertable, chalkBot won't move.
 *                  Negative values will toggle parameter dir
 * \param direction 'b' for backwards, 'f' (or anything else) for forwards.
 * \param speed     currently ignored -> todo 
 */
void move(String distance, String direction, String speed)
{
  debugOut("move");
  
  if(distance.toInt() == 0) 
  {
    UartToESP.print("ERR: ");
    UartToESP.print(INVALID_PARAMETER);
    UartToESP.println(", invalid parameters for move");
    nextInQueue();
  }
  else // start motion
  {
    // set direction (if distance is negative -> toggle direction)
    if(  distance.toInt()<0 || direction == "b" ) 
    {
      xDir = true;
      yDir = false;
    }
    else
    {
      xDir = false;
      yDir = true;
    }
  
    int steps = distance.toInt(); // todo skalieren ( STEPS_PER_M * (unsigned long int)abs(distance.toInt()) ) / 1000 * 2;    
    ticks = 0;
    Serial.print("target Interval: ");
    Serial.println(speedy.go(steps, speed.toInt() ));     
  }

  
}


/**
 * base function to set parameters for turning the car on the spot. 
 * One wheel spinning fw other one bw.
 * 
 * \param degree    in tenth of a degree
 * \param direction 'l': turn left (counterclockwise), 'r' (or anything else): turned right (clockwise)      
 * \param speed     currently ignored -> todo
 
 */
void turn(String degree, String direction, String speed)
{  
   // set direction
   if(degree.toInt() < 0 || direction=="l")
   {
      xDir = false;
      yDir = false;
   }
   else
   {
      xDir = true;
      yDir = true;
   }

  if(degree.toInt() == 0) 
    {
      UartToESP.print("ERR: ");
      UartToESP.print(INVALID_PARAMETER);
      UartToESP.println(", invalid parameters for turn");
      nextInQueue();
    }
    else // start motion
    {
      int steps = degree.toInt(); // todo faktor ( STEPS_PER_TURN * (unsigned long int)abs(degree.toInt()) ) / 3600 * 2;
      ticks = 0;
      speedy.go(steps, speed.toInt() );  
      Serial.println();
    }
}


/**
 * nextInQueue
 * move elements in cmd queue and start processing of first element
 */
void nextInQueue()
{        
  // move elements in queue
  for(int i=0; i<cmdIndex; i++) cmdQueue[i]=cmdQueue[i+1];
  cmdIndex--;

  UartToESP.print("DONE: ");
  UartToESP.println(MAX_CMDS-cmdIndex);

  // call next element if available
  if(cmdIndex>0)  processCmd(cmdQueue[0]);
  // TODO: endtscheiden ob das automatisch passieren soll
  // or disable motors
  // else            digitalWrite(EN, HIGH);
}


void performStep()
{
  // direction
  digitalWrite(X_DIR, xDir);
  digitalWrite(Y_DIR, yDir);

  digitalWrite(EN, LOW);
  digitalWrite(X_STP, HIGH);
  digitalWrite(Y_STP, HIGH);    
}

/**
 * timed_function
 */
void timed_function() 
{    
  // todo reicht ein us für das step signal???
  digitalWrite(X_STP, LOW);
  digitalWrite(Y_STP, LOW);
  
  ticks++;

  if( speedy.isNextStep(ticks) )
  {    
    Serial.print(ticks);
    Serial.println(", ");
    
    ticks = 0;
    performStep();
    speedy.step();
    if( !speedy.stepLeft() ) nextInQueue();  
  }  

  TimerLib.setTimeout_us(timed_function, BASE_CLK_us);
}


/**
 * debugOutput via WIFI
 */
void debugOut(String output)
{
  // UartToESP.println("Debug: " + output);
}



/**
 * main loop
 */
void loop()
{
  // check for data from ESP/WiFi
  if (UartToESP.available() > 0) 
  {
    processInput();    
  }
  
}


