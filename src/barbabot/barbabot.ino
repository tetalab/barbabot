/* 
 BARBABOT
 
 Program to control Barbabot Robotic Cocctail by Tetalab.
 23 syringe (0->22) and 4 bottle (23, 24, 25, 26).
 si bouteille quantité par multiple de 40ml
 si seringue alors exprimé en max 100ml.

 Created 23 Octobre 2014 : V.0
 by Flo Gales with Tetalab.
 
 Modified Minitel library by Minitel library for Arduino (v0.1) / May 2013 http://github.com/01010101/Minitel
 
 License Rien à branler.
 License Do What The Fuck You Want.
 */
 
#include "config.h"

/** VARIABLES */
int carouselEndstopState = HIGH;         // State of endstop of the carousel.
int positionCarousel = 0; // Number of step from endstop of the carousel = position of the carousel.

int syringeEndstopState = HIGH;         // State of endstop of the syringe arm.
int syringeContactStopState = HIGH;         // State of microswitch who push on syringe of the carousel.
int positionSyringe = 0; // Number of step from endstop of the syringe = position of the syringe.

int bottleEndstopState = 0;         // State of endstop of the bottle arm.
int positionBottle = 0; // Number of step from endstop of the arm dedicated to bottle = position of the syringe.

int armState = LOW; //arm microswitch is pressed.

//Minitel minitel;

void setup() 
{                
  pinMode(PIN_STEP_CAROUSEL, OUTPUT);
  pinMode(PIN_DIR_CAROUSEL, OUTPUT); 
  pinMode(PIN_STEP_SYRINGE, OUTPUT);
  pinMode(PIN_DIR_SYRINGE, OUTPUT); 
  pinMode(PIN_STEP_BOTTLE, OUTPUT);
  pinMode(PIN_DIR_BOTTLE, OUTPUT);  
  
  pinMode(PIN_CAROUSEL_ENDSTOP, INPUT);  
  pinMode(PIN_SYRINGE_ENDSTOP, INPUT);
  pinMode(PIN_SYRINGE_CONTACT, INPUT);
  pinMode(PIN_SLOT_ARM, INPUT);
  
  // Serial for debugging
  Serial.begin(9600);
  UCSR1C = B00100100;
  Serial1.begin(1200);
  Serial1.write(12);
  //Init random generator
  randomSeed(analogRead(0));
}


void loop() 
{
  //serviceMode(); //uncomment to activate service mode.
  
  // Init of the bordel
  initSyringe();
  initBottle();
  initCarousel();
  initListeCocktails();
  
  
  //Waiting for slot arm
  Serial1.write(14);
  Serial1.write(12);
  Serial1.write("PULL SLOT ARM TO PLAY.");
  while(armState == 0){
    armState = digitalRead(PIN_SLOT_ARM);
  }
  armState = 0;
  
  //Random cocktail
  int randNumber = random(NUMBER_OF_COCKTAILS);
  Serial1.write(DOWN);Serial1.write(13);
  Serial1.write(13);
  Serial1.write("COCKTAIL :");
  Serial1.write(DOWN);Serial1.write(13);
  Serial1.print(String(" - NAME: ") + String(liste_cocktails[randNumber].name));
  Serial1.write(DOWN);Serial1.write(13);
  Serial1.print(" - AUTHOR: " + liste_cocktails[randNumber].author);
  
  //Let's do it.
  makeCocktail(liste_cocktails[randNumber]);

}

/*
* Serve a cocktail.
*/
void makeCocktail(Cocktail cocktail) 
{
  DEBUG(String("begin make cocktail taille ingredient =") + String(sizeof(cocktail.ingredient[0]), DEC));
  
  //Put each ingredient.
  for(int i=0; i<sizeof(cocktail.ingredient[0]); i++){
    if(cocktail.ingredient[i][0] != NULL){
      Serial1.write(DOWN);
      Serial1.print(String("Serving ") + String(cocktail.ingredient[i][1])+ String("ml of ")+ String(INGREDIENTS[cocktail.ingredient[i][0]]));
      
      int slot = findAlcohol(cocktail.ingredient[i][0], cocktail.ingredient[i][1]);
      if(slot != -1){
        goToCarousel(slot);
        serveAlcohol(slot, cocktail.ingredient[i][1]);
      }else{
        Serial1.write(DOWN);
        Serial1.write("Empty ingredient.Cannot continue. FILLED IT HUMAN!!");
      }
    }
  }
  
  DEBUG("end make cocktail");
}

/*
* Find the correct alcohol from ingredient and remaining quantity.
* @return int number of slot. Return -1 if ingredient is empty
* From 0->24 : syringe
* Form 25->25 : Bottle
*/
int findAlcohol(int alcoolNumber, int quantity) 
{
  DEBUG(String("begin findAlcohol alcoolNumber=") + String(alcoolNumber, DEC) + String ("qty=") + String(quantity, DEC));
  int slot = -1;
  
  //Cycle through alcohol
  for(int i = 0; i < SIZEOF_BAR; i++){
    //DEBUG(String("BAR[i][0]=") + String(BAR[i][0], DEC));
    //DEBUG(String("BAR[i][1]=") + String(BAR[i][1], DEC));
    if(BAR[i][0] == alcoolNumber && BAR[i][1] >= quantity){ //if right alcohol and good amount.
      slot = i; 
    }
  }
  
  DEBUG(String("end findAlcohol") + String(slot, DEC));
  return slot;
}


/*
* Move carousel to specific number.
* From 0->22 : syringe
* Form 23->26 : Bottle
*/
void goToCarousel(int alcoolNumber) 
{
  DEBUG("begin goToCarousel alcoolNumber");
  
  int numberOfStepFromBegin = 0;
  int deltaStep = 0;
  //If it's a syringe.
  if(alcoolNumber<23){
    numberOfStepFromBegin = (alcoolNumber-1) * STEP_TO_SYRINGE;
    DEBUG(String("SERINGUE NUM=") + String(alcoolNumber, DEC));
  }else{
    numberOfStepFromBegin = (alcoolNumber-23) * STEP_TO_BOTTLE;
    DEBUG(String("BOUTEILLE NUM=") + String(alcoolNumber-23, DEC));
  }
  
  DEBUG(String("positionCarousel=") + String(positionCarousel, DEC));
  DEBUG(String("numberOfStepFromBegin=") + String(numberOfStepFromBegin, DEC));
  
  if(numberOfStepFromBegin > positionCarousel){//clockwise turn.
    digitalWrite(PIN_DIR_CAROUSEL, HIGH_CAROUSEL);
    deltaStep = numberOfStepFromBegin - positionCarousel;
  }else{
    digitalWrite(PIN_DIR_CAROUSEL, LOW_CAROUSEL);
    deltaStep = positionCarousel - numberOfStepFromBegin;
  }
  
  DEBUG(String("delta=") + String(deltaStep, DEC));
  
  for(int i=0; i<=deltaStep;i++){
    //Move on
    digitalWrite(PIN_STEP_CAROUSEL, HIGH);   
    delay(SPEED_CAROUSEL);
    digitalWrite(PIN_STEP_CAROUSEL, LOW);
  }
  positionCarousel = numberOfStepFromBegin;
  
  DEBUG("end goToCarousel");
}

/*
* Serve Alcohol (bottle or syringe).
*/
void serveAlcohol(int slot, int quantity) 
{
  DEBUG(String("begin serveAlcohol slot=") + String(slot, DEC));
  
  int numberOfStepFromBegin = 0;
  //If it's a syringe.
  if(slot<23){
    
    //Move until syringe switch
    digitalWrite(PIN_DIR_SYRINGE, HIGH_SYRINGE);
    syringeContactStopState = digitalRead(PIN_SYRINGE_CONTACT);
    DEBUG(String("syringeContactStopState=") + String(syringeContactStopState, DEC));
    while(syringeContactStopState == 1 && numberOfStepFromBegin < MAX_STEP_SYRINGE){
      //Move on
      digitalWrite(PIN_STEP_SYRINGE, HIGH);   
      delay(SPEED_SYRINGE);
      digitalWrite(PIN_STEP_SYRINGE, LOW);
      //Read value of endstop. 
      syringeContactStopState = digitalRead(PIN_SYRINGE_CONTACT);
      numberOfStepFromBegin++;
    }
    
    //Move according quantity.
    for(int i = 0;i<(quantity * STEP_TO_ML);i++){
      if(numberOfStepFromBegin < MAX_STEP_SYRINGE){
        //Move on
         digitalWrite(PIN_STEP_SYRINGE, HIGH);   
         delay(SPEED_SYRINGE);
         digitalWrite(PIN_STEP_SYRINGE, LOW);
         numberOfStepFromBegin++;
      }
    }
    
    DEBUG(String("numberOfStepFromBegin=") + String(numberOfStepFromBegin, DEC));
    
    //decrease quantity in memory.
    BAR[slot][1] -= quantity;
    
    initSyringe();
    
  }else{
    //In cas of bottle (1 push of bottle = 40ml)
    for(int i = 0; i<(quantity/40);i++){
      digitalWrite(PIN_DIR_BOTTLE, HIGH_BOTTLE);
      for(int j = 0; j<STEP_TO_PRESS_BOTTLE; j++){
        //Move on
        digitalWrite(PIN_STEP_BOTTLE, HIGH);   
        delay(SPEED_BOTTLE);
        digitalWrite(PIN_STEP_BOTTLE, LOW);
        positionBottle++;
      }
      initBottle();
    }
    
    //decrease quantity in memory.
    BAR[slot][1] -= quantity;
  }
  
  DEBUG("end serveAlcohol");
}

/*
* Carousel go to 0.
*/
int initCarousel() 
{
  int nbStep = 0;
  DEBUG("begin init carousel");
  
  //While endstop not reach.
  digitalWrite(PIN_DIR_CAROUSEL, LOW_CAROUSEL);
  carouselEndstopState = digitalRead(PIN_CAROUSEL_ENDSTOP);
  while(carouselEndstopState == 1){
    //Move on
    digitalWrite(PIN_STEP_CAROUSEL, HIGH);   
    delay(SPEED_CAROUSEL);
    digitalWrite(PIN_STEP_CAROUSEL, LOW);
    //Read value of endstop. 
    carouselEndstopState = digitalRead(PIN_CAROUSEL_ENDSTOP);
    nbStep++;
  }
  
  //Un petit coup doucement pour le rebondi
  delay(1000);
  carouselEndstopState = digitalRead(PIN_CAROUSEL_ENDSTOP);
  while(carouselEndstopState == 1){
    //Move on
    digitalWrite(PIN_STEP_CAROUSEL, HIGH);   
    delay(100);
    digitalWrite(PIN_STEP_CAROUSEL, LOW);
    //Read value of endstop. 
    carouselEndstopState = digitalRead(PIN_CAROUSEL_ENDSTOP);
  }
  positionCarousel = 0;
  
  DEBUG(String("end init carousel nbStepForRevolutionHastaSiempreCommandante=") + String(nbStep, DEC));
  return nbStep;
}

/*
* Arm for syringe go to 0.
*/
void initSyringe() 
{
  DEBUG("begin init syringe");
  //While endstop not reach.
  digitalWrite(PIN_DIR_SYRINGE, LOW_SYRINGE);
  syringeEndstopState = digitalRead(PIN_SYRINGE_ENDSTOP);
  while(syringeEndstopState == 1){ //reversed microswitch
    //Move on
    digitalWrite(PIN_STEP_SYRINGE, HIGH);   
    delay(SPEED_SYRINGE);
    digitalWrite(PIN_STEP_SYRINGE, LOW);
    //Read value of endstop. 
    syringeEndstopState = digitalRead(PIN_SYRINGE_ENDSTOP);
  }
  positionSyringe = 0;
  DEBUG("end init syringe");
}

/*
* Arm for bottle go to 0.
*/
void initBottle() 
{
  DEBUG("begin init bottle arm");
  //While endstop not reach.
  digitalWrite(PIN_DIR_BOTTLE, LOW_BOTTLE);
  while(positionBottle > 0){
    //Move on
    digitalWrite(PIN_STEP_BOTTLE, HIGH);   
    delay(SPEED_BOTTLE);
    digitalWrite(PIN_STEP_BOTTLE, LOW);
    positionBottle--;
  }
  positionBottle = 0;
  DEBUG("end init bottle arm");
}

/*
* Service mode
*/
void serviceMode() 
{
  DEBUG("SERVICE MODE");
  
  while(1 == 1){
    DEBUG(String("ARM STATE=") + String(digitalRead(PIN_SLOT_ARM), DEC));
    DEBUG(String("MIN SYRINGE=") + String(digitalRead(PIN_SYRINGE_ENDSTOP), DEC));
    DEBUG(String("CONTACT SYRINGE=") + String(digitalRead(PIN_SYRINGE_CONTACT), DEC));
    DEBUG(String("MIN CAROUSEL=") + String(digitalRead(PIN_CAROUSEL_ENDSTOP), DEC));
    delay(3000);
  }
}

void DEBUG(String string){
  if(DEBUG_MODE) Serial.println(string);
  
}

