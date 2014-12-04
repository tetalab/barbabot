#include <Servo.h>
#include "symboles.h"


// define the parameters of our machine.
float X_STEPS_PER_INCH = 48;
float X_STEPS_PER_MM = 10;
int X_MOTOR_STEPS   = 500;

float Y_STEPS_PER_INCH = 48;
float Y_STEPS_PER_MM  = 4;
int Y_MOTOR_STEPS   = 500;

float Z_STEPS_PER_INCH = 48;
float Z_STEPS_PER_MM   = 40;
int Z_MOTOR_STEPS    = 100;

//our maximum feedrates
long FAST_XY_FEEDRATE = 2000;
long FAST_Z_FEEDRATE = 2000;

// Units in curve section
#define CURVE_SECTION_INCHES 0.019685
#define CURVE_SECTION_MM 0.5

// Set to one if sensor outputs inverting (ie: 1 means open, 0 means closed)
// RepRap opto endstops are *not* inverting.
int SENSORS_INVERTING = 1;

// How many temperature samples to take.  each sample takes about 100 usecs.


/****************************************************************************************
* digital i/o pin assignment
*
* this uses the undocumented feature of Arduino - pins 14-19 correspond to analog 0-5
****************************************************************************************/

int X_STEP_PIN = 25;
int X_DIR_PIN = 24;
int X_ENABLE_PIN = 4;
int X_MIN_PIN = A4;
int X_MAX_PIN = A5;

int Y_STEP_PIN = 27;
int Y_DIR_PIN = 26;
int Y_ENABLE_PIN = 4;
int Y_MIN_PIN = A1;
int Y_MAX_PIN = A0;

int Z_STEP_PIN = 40;//A3 => 40
int Z_DIR_PIN = 9;
int Z_ENABLE_PIN = 4;
int Z_MIN_PIN = A1;
int Z_MAX_PIN = A0;
int Z_ENABLE_SERVO = 1;
#define COMMAND_SIZE 128

char phrase[30];
//char commands[COMMAND_SIZE];
byte serial_count;
int no_data = 0;

Servo servo;

int currentPosServo = 2;
int targetPosServo = 2;
bool comment = false;

#include "cupbot.h"

void setup()
{
	//Do startup stuff here
        Serial.begin(9600);
	Serial1.begin(115200);
        if(Z_ENABLE_SERVO==1){
          servo.attach(Z_STEP_PIN);
          servo.write(-1);
        }
	//other initialization.
	init_process_string();
	init_steppers();
	process_string("G90",3);//Absolute Position
        //Serial.println("start");
        Serial1.println("start");
        memset(phrase, 0, 30);
}

void loop()
{
    
        char c;
	//read in characters if we got them.
	while (Serial1.available() > 0)
	{
		c = Serial1.read();
                
		no_data = 0;
		//newlines are ends of commands.
		if (c != '\n')
		{
			if(c==0x18){
				Serial1.println("Grbl 1.0");
			}else{
                          if (c == '('){
                            comment = true;
                          }
                          // If we're not in comment mode, add it to our array.
                          if (!comment)
                          {
                            phrase[serial_count] = c;
                    	    serial_count++;
                          }
                          if (c == ')'){
                            comment = false; // End of comment - start listening again
                          }
                        }
				
		}else{
                  if (serial_count > 0 && (c == '\n'))// || no_data > 100))
          	  {
                          Serial1.println("process");
                          //Serial.println(phrase);
                          generateAndSendGCode(phrase);
                          memset(phrase, 0, 30);
                          //Serial.println("fin");
          		//process our command!
          		
          	  }
            }
	}       
}
