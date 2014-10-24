/* 
 BARBABOT
 
 Program to control Barbabot Robotic Cocctail by Tetalab.
 25 syringe (0->24) and 4 bottle (25, 26, 27, 28).
 
 Created 23 Octobre 2014 : V.0
 by Flo Gales
 
 License Rien à branler.
 License Do What The Fuck You Want.
 */

#include <Stepper.h>

const int STEP_PER_REVOLUTION = 200; //Default number of steps by revolution.

/** PINS and CONSTANTS **/

//CAROUSEL
const int PIN_STEP_CAROUSEL = 12;  //Pin step carousel
const int PIN_DIR_CAROUSEL = 13;   //Pin direction carousel
const int PIN_CAROUSEL_ENDSTOP = 18;
const int HIGH_CAROUSEL = HIGH;
const int LOW_CAROUSEL = LOW;
int carouselEndstopState = 0;         // State of endstop of the carousel.
int positionCarousel = 0; // Number of step from endstop of the carousel = position of the carousel.
int stepToSyringe = 8; // Number of step to reach a syringe.
int stepToBottle = 50; // Number of step to reach a bottle.
int speedCarousel = 10; //delay between each step.

//SYRINGE
const int PIN_STEP_SYRINGE = 14;  //Pin step syringe
const int PIN_DIR_SYRINGE = 15;   //Pin direction syringe
const int PIN_SYRINGE_ENDSTOP = 19;
const int HIGH_SYRINGE = HIGH;
const int LOW_SYRINGE = LOW;
const int PIN_SYRINGE_CONTACT = 20;
const int STEP_TO_ML = 2; //Number of step for 1 ml
int maxStepSyringe = 0; // maximum step.
int speedSyringe = 10; //delay between each step.
int syringeEndstopState = 0;         // State of endstop of the syringe arm.
int syringeContactStopState = 0;         // State of microswitch who push on syringe of the carousel.
int positionSyringe = 0; // Number of step from endstop of the syringe = position of the syringe.

//BOTTLE
const int PIN_STEP_BOTTLE = 16;  //Pin step bottle
const int PIN_DIR_BOTTLE = 17;   //Pin direction bottle
const int PIN_BOTTLE_ENDSTOP = 21;
const int HIGH_BOTTLE = HIGH;
const int LOW_BOTTLE = LOW;
const int STEP_TO_PRESS_BOTTLE = 20; // Number of step to have 4cl from bottle.
int bottleEndstopState = 0;         // State of endstop of the bottle arm.
int positionBottle = 0; // Number of step from endstop of the arm dedicated to bottle = position of the syringe.
int maxStepBottle = 0; // maximum step.
int speedBottle = 10; //delay between each step.

//ARM
const int PIN_SLOT_ARM = 22;
int armState = 0; //arm microswitch is pressed.

/** COCKTAILS **/
//10 cocktails max avec 5 ingrédients max chacun.
int cocktails[10][5]={
  {0,1,2,3,4},
  {1,2,3,4,5},
  {2,3,4,5,6}
};

void setup() 
{                
  pinMode(PIN_STEP_CAROUSEL, OUTPUT);
  pinMode(PIN_DIR_CAROUSEL, OUTPUT); 
  pinMode(PIN_STEP_SYRINGE, OUTPUT);
  pinMode(PIN_DIR_SYRINGE, OUTPUT); 
  pinMode(PIN_STEP_BOTTLE, OUTPUT);
  pinMode(PIN_DIR_BOTTLE, OUTPUT);  
  
  pinMode(PIN_CAROUSEL_ENDSTOP, INPUT);  
  
  // Init Serial port:
  Serial.begin(9600);
  
  //Init random generator
  randomSeed(analogRead(0));
}

void loop() 
{
  //Init of the bordel
  initCarousel();
  initSyringe();
  initBottle();
  
  //Waiting for slot arm
  while(armState == 0){
    armState = digitalRead(PIN_SLOT_ARM);
  }
  
  //Random cocktail
  int randNumber = random(sizeof(cocktails));
  Serial.println("COCKTAIL => ");
  Serial.println(randNumber);
  
  //Let's do it.
  makeCocktail(randNumber);
}

/*
* Carousel go to 0.
*/
void makeCocktail(int cocktailNumber) 
{
  //Récup recette.
  int recette[sizeof(cocktails)] = {cocktails[cocktailNumber][0], cocktails[cocktailNumber][1], cocktails[cocktailNumber][2], cocktails[cocktailNumber][3], cocktails[cocktailNumber][4]}; //Parce que j'ai horreur des pointeurs.
  //Put each ingredient.
  for(int i=0;i<sizeof(cocktails);i++){
    goToCarousel(cocktailNumber);
    serveAlcohol(cocktailNumber, recette[i]);
  }
  Serial.println("end make cocktail");
}

/*
* Move carousel to specific number.
*/
void goToCarousel(int alcoolNumber) 
{
  Serial.println("begin goToCarousel");
  int numberOfStepFromBegin = 0;
  //If it's a syringe.
  if(alcoolNumber<25){
    numberOfStepFromBegin = numberOfStepFromBegin * stepToSyringe;
  }else{
    numberOfStepFromBegin = numberOfStepFromBegin * stepToBottle;
  }
  
  if(numberOfStepFromBegin < positionCarousel){//clockwise turn.
    digitalWrite(PIN_DIR_CAROUSEL, HIGH_CAROUSEL);
  }else{
    digitalWrite(PIN_DIR_CAROUSEL, LOW_CAROUSEL);
  }
  
  while(positionCarousel != numberOfStepFromBegin){
    //Move on
    digitalWrite(PIN_STEP_CAROUSEL, HIGH_CAROUSEL);   
    delay(speedCarousel);
    digitalWrite(PIN_STEP_CAROUSEL, LOW_CAROUSEL);
    
    positionCarousel++;
  }
  
  Serial.println("end goToCarousel");
}

/*
* Move carousel to specific number.
*/
void serveAlcohol(int alcoolNumber, int quantity) 
{
  Serial.println("begin serveAlcohol");
  int numberOfStepFromBegin = 0;
  //If it's a syringe.
  if(alcoolNumber<25){
    
    //Move until syringe switch
    digitalWrite(PIN_DIR_SYRINGE, HIGH_SYRINGE);
    while(syringeContactStopState == 0){
      //Move on
      digitalWrite(PIN_STEP_SYRINGE, HIGH);   
      delay(speedSyringe);
      digitalWrite(PIN_STEP_SYRINGE, LOW);
      //Read value of endstop. 
      syringeContactStopState = digitalRead(PIN_SYRINGE_CONTACT);
    }
    
    //TODO : Ajouter check si seringue vide.
    
    //Move according quantity.
    for(int i = 0;i<(quantity/STEP_TO_ML);i++){
      //Move on
      digitalWrite(PIN_STEP_SYRINGE, HIGH);   
      delay(speedSyringe);
      digitalWrite(PIN_STEP_SYRINGE, LOW);
    }
    
    initSyringe();
    
  }else{
    //In cas of bottle quantity = 1 push of bottle = 4cl
    for(int i = 0; i<quantity;i++){
      for(int j = 0; j<STEP_TO_PRESS_BOTTLE; j++){
        //Move on
        digitalWrite(PIN_STEP_SYRINGE, HIGH);   
        delay(speedBottle);
        digitalWrite(PIN_STEP_SYRINGE, LOW);
      }
      initBottle();
    }
  }
  
  Serial.println("end serveAlcohol");
}

/*
* Carousel go to 0.
*/
void initCarousel() 
{
  Serial.println("begin init carousel");
  //While endstop not reach.
  digitalWrite(PIN_DIR_CAROUSEL, LOW_CAROUSEL);
  while(carouselEndstopState == 0){
    //Move on
    digitalWrite(PIN_STEP_CAROUSEL, HIGH);   
    delay(speedCarousel);
    digitalWrite(PIN_STEP_CAROUSEL, LOW);
    //Read value of endstop. 
    carouselEndstopState = digitalRead(PIN_CAROUSEL_ENDSTOP);
  }
  positionCarousel = 0;
  Serial.println("end init carousel");
}

/*
* Arm for syringe go to 0.
*/
void initSyringe() 
{
  Serial.println("begin init syringe");
  //While endstop not reach.
  digitalWrite(PIN_DIR_SYRINGE, LOW_SYRINGE);
  while(syringeEndstopState == 0){
    //Move on
    digitalWrite(PIN_STEP_SYRINGE, HIGH);   
    delay(speedSyringe);
    digitalWrite(PIN_STEP_SYRINGE, LOW);
    //Read value of endstop. 
    syringeEndstopState = digitalRead(PIN_SYRINGE_ENDSTOP);
  }
  positionSyringe = 0;
  syringeContactStopState = 0;
  Serial.println("end init syringe");
}

/*
* Arm for bottle go to 0.
*/
void initBottle() 
{
  Serial.println("begin init bottle arm");
  //While endstop not reach.
  digitalWrite(PIN_DIR_BOTTLE, LOW_BOTTLE);
  while(bottleEndstopState == 0){
    //Move on
    digitalWrite(PIN_STEP_BOTTLE, HIGH);   
    delay(speedBottle);
    digitalWrite(PIN_STEP_BOTTLE, LOW);
    //Read value of endstop. 
    bottleEndstopState = digitalRead(PIN_BOTTLE_ENDSTOP);
  }
  positionBottle = 0;
  Serial.println("end init bottle arm");
}

