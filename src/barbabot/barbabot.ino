/* 
 BARBABOT
 
 Program to control Barbabot Robotic Cocctail by Tetalab.
 25 syringe (0->24) and 4 bottle (25, 26, 27, 28).
 si bouteille quantité par 40ml (donc qté 2 = 80ml)
 si seringue alors exprimé en ml.

 Created 23 Octobre 2014 : V.0
 by Flo Gales
 
 License Rien à branler.
 License Do What The Fuck You Want.
 */
 
#include "config.h"

/** VARIABLES */
int carouselEndstopState = 0;         // State of endstop of the carousel.
int positionCarousel = 0; // Number of step from endstop of the carousel = position of the carousel.

int syringeEndstopState = 0;         // State of endstop of the syringe arm.
int syringeContactStopState = 0;         // State of microswitch who push on syringe of the carousel.
int positionSyringe = 0; // Number of step from endstop of the syringe = position of the syringe.

int bottleEndstopState = 0;         // State of endstop of the bottle arm.
int positionBottle = 0; // Number of step from endstop of the arm dedicated to bottle = position of the syringe.

int armState = 0; //arm microswitch is pressed.



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
  
  Serial.println("PULL SLOT ARM TO PLAY. FUCKER!");
  
  //Waiting for slot arm
  while(armState == 0){
    armState = digitalRead(PIN_SLOT_ARM);
  }
  
  //Random cocktail
  int randNumber = random(sizeof(liste_cocktails));
  
  Serial.println("RANDOM COCKTAIL :");
  Serial.println(" - NAME: " + liste_cocktails[randNumber].name);
  Serial.println(" - AUTHOR: " + liste_cocktails[randNumber].author);
  
  //Let's do it.
  makeCocktail(liste_cocktails[randNumber]);
}

/*
* Serve a cocktail.
*/
void makeCocktail(Cocktail& cocktail) 
{
  Serial.println("begin make cocktail");
  //Put each ingredient.
  for(int i=0; i<sizeof(cocktail.ingredient); i++){
    int slot = findAlcohol(cocktail.ingredient[i][0], cocktail.ingredient[i][1]);
    if(slot != -1){
      goToCarousel(slot);
      serveAlcohol(slot, cocktail.ingredient[i][1]);
    }else{
      Serial.println("Empty ingredient.Cannot continue. FILLED IT HUMAN!!");
    }
  }
  Serial.println("end make cocktail");
}

/*
* Find the correct alcohol from ingredient and remaining quantity.
* @return int number of slot. Return -1 if ingredient is empty
* From 0->24 : syringe
* Form 25->25 : Bottle
*/
int findAlcohol(int alcoolNumber, int quantity) 
{
  Serial.println("begin findAlcohol");
  int slot = -1;
  
  //Cycle through alcohol
  for(int i = 0; i < sizeof(BAR); i++){
    if(BAR[i][0] == alcoolNumber && BAR[i][1] >= quantity){ //if right alcohol and good amount.
      slot = i; 
    }
  }
  
  Serial.println("end findAlcohol");
  return slot;
}

/*
* Move carousel to specific number.
* From 0->24 : syringe
* Form 25->25 : Bottle
*/
void goToCarousel(int alcoolNumber) 
{
  Serial.println("begin goToCarousel");
  int numberOfStepFromBegin = 0;
  //If it's a syringe.
  if(alcoolNumber<25){
    numberOfStepFromBegin = numberOfStepFromBegin * STEP_TO_SYRINGE;
  }else{
    numberOfStepFromBegin = numberOfStepFromBegin * STEP_TO_BOTTLE;
  }
  
  if(numberOfStepFromBegin < positionCarousel){//clockwise turn.
    digitalWrite(PIN_DIR_CAROUSEL, HIGH_CAROUSEL);
  }else{
    digitalWrite(PIN_DIR_CAROUSEL, LOW_CAROUSEL);
  }
  
  while(positionCarousel != numberOfStepFromBegin){
    //Move on
    digitalWrite(PIN_STEP_CAROUSEL, HIGH_CAROUSEL);   
    delay(SPEED_CAROUSEL);
    digitalWrite(PIN_STEP_CAROUSEL, LOW_CAROUSEL);
    
    positionCarousel++;
  }
  
  Serial.println("end goToCarousel");
}

/*
* Serve Alcohol (bottle or syringe).
*/
void serveAlcohol(int slot, int quantity) 
{
  Serial.println("begin serveAlcohol");
  int numberOfStepFromBegin = 0;
  //If it's a syringe.
  if(slot<25){
    
    //Move until syringe switch
    digitalWrite(PIN_DIR_SYRINGE, HIGH_SYRINGE);
    while(syringeContactStopState == 0){
      //Move on
      digitalWrite(PIN_STEP_SYRINGE, HIGH);   
      delay(SPEED_SYRINGE);
      digitalWrite(PIN_STEP_SYRINGE, LOW);
      //Read value of endstop. 
      syringeContactStopState = digitalRead(PIN_SYRINGE_CONTACT);
    }
   
    //Move according quantity.
    for(int i = 0;i<(quantity/STEP_TO_ML);i++){
      //Move on
      digitalWrite(PIN_STEP_SYRINGE, HIGH);   
      delay(SPEED_SYRINGE);
      digitalWrite(PIN_STEP_SYRINGE, LOW);
    }
    //decrease quantity in memory.
    BAR[slot][1] -= quantity;
    
    initSyringe();
    
  }else{
    //In cas of bottle (1 push of bottle = 40ml)
    for(int i = 0; i<(quantity/40);i++){
      for(int j = 0; j<STEP_TO_PRESS_BOTTLE; j++){
        //Move on
        digitalWrite(PIN_STEP_SYRINGE, HIGH);   
        delay(SPEED_BOTTLE);
        digitalWrite(PIN_STEP_SYRINGE, LOW);
      }
      initBottle();
    }
    
    //decrease quantity in memory.
    BAR[slot][1] -= quantity;
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
    delay(SPEED_CAROUSEL);
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
    delay(SPEED_SYRINGE);
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
    delay(SPEED_BOTTLE);
    digitalWrite(PIN_STEP_BOTTLE, LOW);
    //Read value of endstop. 
    bottleEndstopState = digitalRead(PIN_BOTTLE_ENDSTOP);
  }
  positionBottle = 0;
  Serial.println("end init bottle arm");
}

