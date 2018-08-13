#include <SoftwareSerial.h>

#include <uTimerLib.h>

// Hier kommen die Kommandos (per ESP/Wifi) her:
SoftwareSerial UartToESP(11,10); // Rx, Tx


#define EN        8       //The step motor makes the power end, low level is effective
#define X_DIR     5       //The x-axis moves in the direction of the motor
#define Y_DIR     6       //The y-axis moves in the direction of the motor

#define X_STP     2       //The x axis stepper control
#define Y_STP     3       //The y axis stepper control

/*
 * Timer for performing one step will be called n times before step is done
 */
typedef enum{
  stay  = 0,
  crawl = 10,
  walk  = 4,
  jog   = 3,
  bolt  = 2
}
Speed_t;


bool runX = true;
bool runY = true;
bool onOff = true; 
bool xDir = true;
bool yDir = true;
long int timerCall_us = 500;
int counter = 0;
Speed_t carSpeed = stay;
bool singleStep = false;
String inputString = "";
const int MAX_PARAMS = 5;
String param[MAX_PARAMS];



/**
 * setup
 */
void setup()
{

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
 * base function to set the robot movement parameters
 * which will be used with the next timer call
 * 
 * \param distance_mm way to go in milimeter
 * \param speed can be stay, crawl, walk, jog or bolt. \n
 *              If it is stay, all other parameters will be ignored.
 * \param forward when true, backward when false
 */
void move(Speed_t speed, unsigned int distance_mm, bool forward)
{
  carSpeed = speed;
  if(forward) xDir = yDir = true;
  else        xDir = yDir = false;

}

/**
 * base function to set parameters for turning the car on the spot. 
 * One wheel spinning vw other one bw.
 * 
 * \param speed can be stay, crawl, walk, jog or bolt. \n
 *              If it is stay, all other parameters will be ignored.\n
 *              bolt is probably not recommended for turning.
 * \param degree in 1 degree steps (todo: test and maybe refine to tenth of a degree)
 * \param direction true=clockwise, false=counterclockwise
 */
void turn(Speed_t speed, unsigned int degree, bool direction)
{
   carSpeed = speed;
   xDir=true;
   yDir=true;
   if(direction) xDir = false;
   else          yDir = false;
   
}


void timed_function() 
{
  // direction (todo)
  digitalWrite(Y_DIR, true);
  digitalWrite(X_DIR, true);

  // distance (todo)



  // speed
  if( carSpeed == stay  )
  {
    digitalWrite(EN, HIGH); 
  }
  else 
  {
    digitalWrite(EN, LOW);
    
    // move if counter can be devided by desired speed
    if( onOff && (counter%carSpeed) == 0)  
    {   
      digitalWrite(X_STP, HIGH);
      digitalWrite(Y_STP, HIGH);
    }
    else
    {
      digitalWrite(X_STP, LOW);
      digitalWrite(Y_STP, LOW);
    }
  
    onOff = !onOff;
    if(counter>=crawl) counter = 0;
    counter++;  
  }
  
  // restart timer
  TimerLib.setTimeout_us(timed_function, timerCall_us);
}

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

        // split string at "," to get parameters       
        int from = endOfCmd+1;
        for(int i=0; i<MAX_PARAMS; i++)
        {
          if(from>0)
          {
            int endOfParam = inputString.indexOf(",", from);            
            param[i] = inputString.substring(from,endOfParam);           
            param[i].trim();
            from = endOfParam+1;
          }         
          else param[i] =""; 
        }

        // Debug output
        for(int i=0; i<MAX_PARAMS; i++)
        {         
          Serial.print("param :");
          Serial.print(i);
          Serial.print(": ");
          Serial.println(param[i]);
        }

        // list of all valid commands - todo use
        if     (cmd=="stop")    Serial.println("stop");  
        else if(cmd=="move")    Serial.println("move"); 
        else if(cmd=="turn")    Serial.println("turn"); 
        else if(cmd=="chalk")   Serial.println("chalk");
        else if(cmd=="version") Serial.println("version");
        else UartToESP.println("ERR:invalid command"); 

        // clear for new input
        inputString = "";
      }
  }  
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



