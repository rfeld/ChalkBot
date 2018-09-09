#include <SoftwareSerial.h>
#include <uTimerLib.h>

// Hier kommen die Kommandos (per ESP/Wifi) her:
SoftwareSerial UartToESP(11,10); // Rx, Tx
String inputString = "";
const unsigned int  MAX_CMDS = 3;
String              cmdQueue[MAX_CMDS];
unsigned int        cmdIndex=0;

// GPIO definitions
#define EN        8       //The step motor makes the power end, low level is effective
#define X_DIR     5       //The x-axis moves in the direction of the motor
#define Y_DIR     6       //The y-axis moves in the direction of the motor
#define X_STP     2       //The x axis stepper control
#define Y_STP     3       //The y axis stepper control


long int timerCall_us = 1000000;
bool xDir = true;
bool yDir = true;
bool xStep = true;
bool yStep = true;
int  steps;




/**
 * setup
 */
void setup()
{
  steps = 0;
  TimerLib.setTimeout_us(timed_function, timerCall_us);
  
  // set up the IO tube feet used by the motor to be set to output
  pinMode(X_DIR, OUTPUT); pinMode(X_STP, OUTPUT);
  pinMode(Y_DIR, OUTPUT); pinMode(Y_STP, OUTPUT);
  pinMode(EN, OUTPUT);

  // Prepare to use the LED
  pinMode(13,OUTPUT);
  
  digitalWrite(EN, LOW);
  
  Serial.begin(115200);
  UartToESP.begin(9600);
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
          UartToESP.print("Stop ACK ");
          UartToESP.println(MAX_CMDS);
          cmdIndex=0;          
        }
        else
        {          
          // add to command queue
          if(cmdIndex<MAX_CMDS) 
          {
            cmdQueue[cmdIndex++] = inputString;
            UartToESP.print("ACK ");
            UartToESP.println(MAX_CMDS-cmdIndex);
          }
          else  UartToESP.println("ERR: command queue full");
  
          if(cmdIndex==1) processCmd(inputString);
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
    else if(cmd=="chalk")   Serial.println("chalk");
    else if(cmd=="version") Serial.println("version");
    else
    {
      UartToESP.println("ERR:invalid command"); 
  
      // remove from command queue
      nextInQueue();
    }
}

/**
 * stop -> todo
 */
void stop()
{
  debugOut("stop");
  steps=0;
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

  // set direction (if distance is negative -> toggle direction)
  if(  distance.toInt()<0 || direction == "b" )  xDir = yDir = true;
  else                                           xDir = yDir = false;

  steps = abs(distance.toInt());  
}


/**
 * base function to set parameters for turning the car on the spot. 
 * One wheel spinning fw other one bw.
 * 
 * \param degree    in 1 degree steps (todo: test and maybe refine to tenth of a degree)
 * \param direction 'l': turn left (counterclockwise), 'r'(or anything else): turned right (clockwise)      
 * \param speed     currently ignored -> todo
 
 */
void turn(String degree, String direction, String speed)
{
   xDir=true;
   yDir=true;
   if(direction == "l") xDir = false;
   else                 yDir = false;

    steps = abs(degree.toInt()); 
}


/**
 * nextInQueue
 * move elements in cmd queue and start processing of first element
 */
void nextInQueue()
{      
  // move elements in queue // todo: Müllt das meinen Speicher zu???
  for(int i=0; i<cmdIndex; i++) cmdQueue[i]=cmdQueue[i+1];
  cmdIndex--;

  UartToESP.print("DONE ");
  UartToESP.println(MAX_CMDS-cmdIndex);

  // call next element if available
  if(cmdIndex>0) processCmd(cmdQueue[0]);
}


/**
 * timed_function
 */
void timed_function() 
{  
  // direction
  digitalWrite(X_DIR, xDir);
  digitalWrite(Y_DIR, yDir);

  // full speed
  xStep=!xStep;
  yStep=!yStep;  
  

  // decrease distance
  if(steps>0) 
  {
    digitalWrite(EN, LOW);
    digitalWrite(X_STP, xStep);
    digitalWrite(Y_STP, yStep);
    
    // Debug: Visualise motion    
    if      (xDir==false && yDir==false) Serial.print(">");
    else if (xDir==true  && yDir==true ) Serial.print("<");
    else if (xDir==false && yDir==true ) Serial.print("l");
    else if (xDir==true  && yDir==false) Serial.print("r");
    
    steps--;

    // command is now completely processed
    if(steps==0) nextInQueue();    
  }
  else  
  {
    digitalWrite(EN, HIGH);
    Serial.print(".");
  }
  
  // restart timer
  TimerLib.setTimeout_us(timed_function, timerCall_us);
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



