/* 
 BARBABOT
 
 Program to control Barbabot Robotic Cocctail by Tetalab.
 23 syringe (0->22) and 4 bottle (23, 24, 25, 26).
 si bouteille quantité par 40ml (donc qté 2 = 80ml)
 si seringue alors exprimé en ml.

 Created 23 Octobre 2014 : V.0
 by Flo Gales
 
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
  
  // Init Serial port:
  Serial.begin(9600);
  
  //Init random generator
  randomSeed(analogRead(0));
}

void loop() 
{
  //serviceMode(); //uncomment to activate service mode.
  
  //Init of the bordel
  initCarousel();
  initSyringe();
  initBottle();
  initListeCocktails();
  
  Serial.println("PULL SLOT ARM TO PLAY. FUCKER!");
  
  //Waiting for slot arm
  while(armState == 0){
    armState = digitalRead(PIN_SLOT_ARM);
  }
  armState = 0;
  
  //Random cocktail
  int randNumber = random(NUMBER_OF_COCKTAILS);
  Serial.println("RANDOM COCKTAIL :");Serial.println(randNumber);
  Serial.println(" - NAME: " + liste_cocktails[randNumber].name);
  Serial.println(" - AUTHOR: " + liste_cocktails[randNumber].author);
  
  //Let's do it.
  makeCocktail(liste_cocktails[randNumber]);
}

/*
* Service mode
*/
void serviceMode() 
{
  Serial.println("SERVICE MODE");
  
  while(1 == 1){
    Serial.println("ARM STATE="); Serial.println(digitalRead(PIN_SLOT_ARM));
    Serial.println("MIN SYRINGE="); Serial.println(digitalRead(PIN_SYRINGE_ENDSTOP));
    Serial.println("CONTACT SYRINGE="); Serial.println(digitalRead(PIN_SYRINGE_CONTACT));
    Serial.println("MIN CAROUSEL="); Serial.println(digitalRead(PIN_CAROUSEL_ENDSTOP));
    delay(3000);
  }
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
      DEBUG(String("Serving ") + String(cocktail.ingredient[i][1])+ String("ml of ")+ String(INGREDIENTS[cocktail.ingredient[i][0]]));
      
      int slot = findAlcohol(cocktail.ingredient[i][0], cocktail.ingredient[i][1]);
      if(slot != -1){
        goToCarousel(slot);
        serveAlcohol(slot, cocktail.ingredient[i][1]);
      }else{
        Serial.println("Empty ingredient.Cannot continue. FILLED IT HUMAN!!");
      }
    }
  }
  
  if(DEBUG_MODE) Serial.println("end make cocktail");
}

/*
* Find the correct alcohol from ingredient and remaining quantity.
* @return int number of slot. Return -1 if ingredient is empty
* From 0->24 : syringe
* Form 25->25 : Bottle
*/
int findAlcohol(int alcoolNumber, int quantity) 
{
  if(DEBUG_MODE) Serial.println(String("begin findAlcohol alcoolNumber=") + String(alcoolNumber, DEC) + String ("qty=") + String(quantity, DEC));
  int slot = -1;
  
  //Cycle through alcohol
  for(int i = 0; i < SIZEOF_BAR; i++){
    //DEBUG(String("BAR[i][0]=") + String(BAR[i][0], DEC));
    //DEBUG(String("BAR[i][1]=") + String(BAR[i][1], DEC));
    if(BAR[i][0] == alcoolNumber && BAR[i][1] >= quantity){ //if right alcohol and good amount.
      slot = i; 
    }
  }
  
  if(DEBUG_MODE) Serial.println(String("end findAlcohol") + String(slot, DEC));
  return slot;
}

void DEBUG(String string){
  if(DEBUG_MODE) Serial.println(string);
  
}
/*
* Move carousel to specific number.
* From 0->22 : syringe
* Form 23->26 : Bottle
*/
void goToCarousel(int alcoolNumber) 
{
  DEBUG("begin goToCarousel");
  int numberOfStepFromBegin = 0;
  int deltaStep = 0;
  //If it's a syringe.
  if(alcoolNumber<23){
    numberOfStepFromBegin = alcoolNumber * STEP_TO_SYRINGE;
  }else{
    numberOfStepFromBegin = (alcoolNumber-23) * STEP_TO_BOTTLE;
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
  
  if(DEBUG_MODE) Serial.println("end goToCarousel");
}

/*
* Serve Alcohol (bottle or syringe).
*/
void serveAlcohol(int slot, int quantity) 
{
  if(DEBUG_MODE) Serial.println("begin serveAlcohol");
  
  int numberOfStepFromBegin = 0;
  //If it's a syringe.
  if(slot<23){
    
    //Move until syringe switch
    digitalWrite(PIN_DIR_SYRINGE, HIGH_SYRINGE);
    while(syringeContactStopState == 1){
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
  
  if(DEBUG_MODE) Serial.println("end serveAlcohol");
}

/*
* Carousel go to 0.
*/
int initCarousel() 
{
  int nbStep = 0;
  if(DEBUG_MODE) Serial.println("begin init carousel");
  //While endstop not reach.
  digitalWrite(PIN_DIR_CAROUSEL, LOW_CAROUSEL);
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
  if(DEBUG_MODE) Serial.println("begin init syringe");
  //While endstop not reach.
  digitalWrite(PIN_DIR_SYRINGE, LOW_SYRINGE);
  while(syringeEndstopState == 1){ //reversed microswitch
    //Move on
    digitalWrite(PIN_STEP_SYRINGE, HIGH);   
    delay(SPEED_SYRINGE);
    digitalWrite(PIN_STEP_SYRINGE, LOW);
    //Read value of endstop. 
    syringeEndstopState = digitalRead(PIN_SYRINGE_ENDSTOP);
  }
  positionSyringe = 0;
  syringeContactStopState = 0;
  if(DEBUG_MODE) Serial.println("end init syringe");
}

/*
* Arm for bottle go to 0.
*/
void initBottle() 
{
  if(DEBUG_MODE) Serial.println("begin init bottle arm");
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
  if(DEBUG_MODE) Serial.println("end init bottle arm");
}

