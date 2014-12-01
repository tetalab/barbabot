/* 
 CUPBOT
 
 Program to draw dick on a plastic cup
 Tested with arduino Mega on Serial1 with other Mega who run modified MakerBlock XY Plotter firmware (modified to run on Serial2).
 Created 27 November 2014 : V.0
 by Flo Gales.

 License Rien à branler.
 License Do What The Fuck You Want.
 */

#include <Servo.h>
#include "cupbot.h"

// define the parameters of our machine.
float X_STEPS_PER_INCH = 48;
float X_STEPS_PER_MM = 40;
int X_MOTOR_STEPS   = 100;

float Y_STEPS_PER_INCH = 48;
float Y_STEPS_PER_MM  = 40;
int Y_MOTOR_STEPS   = 100;

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

int Z_STEP_PIN = 10;//A3 => 40
int Z_DIR_PIN = 9;
int Z_ENABLE_PIN = 4;
int Z_MIN_PIN = A1;
int Z_MAX_PIN = A0;
int Z_ENABLE_SERVO = 1;
#define COMMAND_SIZE 128

char commands[COMMAND_SIZE];
byte Serial2_count;
int no_data = 0;

Servo servo;

int currentPosServo = 90;
int targetPosServo = 90;
bool comment = false;
void setup()
{
    //Do startup stuff here
    Serial.begin(9600);
    Serial1.begin(115200);
    if(Z_ENABLE_SERVO==1){
      servo.attach(Z_STEP_PIN);
    }
    //other initialization.
    init_process_string();
    init_steppers();
    process_string("G90",3);//Absolute Position
    Serial1.println("start");
    Serial.println("go");
}

void loop()
{
      char* texte[3]={"ROBOEXOTICA 2015", "TOTO SOLDE", "80ML OF VODKA"};
      char* gcode;
      
      //texte = waitForText();
      int i = 0;
      Serial.println("loop");
      while(strlen(texte[i]) > 0 && i < 3){
        //génération du gcode lettre par lettre et exécution.
        for(int iLettre = 0; iLettre < strlen(texte[i]); iLettre++){
          
          boolean bNewLine = iLettre == 0 && i > 0 ? true : false;
          char* gcode = generateGCodeStream(texte[i][iLettre], bNewLine);
          
          char c;
          
          //Lecture du gcode produit.
          for(int j = 0; j < sizeof(gcode); j++)
          {
            c = gcode[j];
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
                    commands[Serial2_count] = c;
                    Serial2_count++;
                  }
                  if (c == ')'){
                    comment = false; // End of comment - start listening again
                  }
                }
            }else{
              //process our command!
              //process_string(commands, Serial2_count);
              //clear command.
              //init_process_string();
            }
          }
        }
        i++;
      }
      //disable_steppers();
      delay(3000);
   
}

char* waitForText(){
  char* lignes[13];
  int i = 0;
  int j = 0;
  boolean bEOT = false;
  
  while (!bEOT){
    if(Serial1.available() > 0){
      char c = Serial1.read();
      if(c == 4){//EOT fin de transmission.
        bEOT = true;
      }else{
        lignes[i][j]= c;
        if(c == '\n'){
          i++;
          j = 0;
        }else{
          j++;
        }
      }
    }
  }
}
