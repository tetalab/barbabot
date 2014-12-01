/* 
 BARBABOT
 
 Program to control Barbabot Robotic Cocctail by Tetalab.
 23 syringe (0->22) and 4 bottle (23, 24, 25, 26).
 si bouteille quantité par multiple de 40ml
 si seringue alors exprimé en max 100ml.

 Created 23 Octobre 2014 : V.0
 by Flo Gales.
 
 Modified Minitel library by Minitel library for Arduino (v0.1) / May 2013 http://github.com/01010101/Minitel
 
 License Rien à branler.
 License Do What The Fuck You Want.
 */

#include "config.h"
#include <SD.h>
#include "sauvegarde.h"

/** VARIABLES */
int positionCarousel = 0; // Number of step from endstop of the carousel = position of the carousel.
int positionSyringe = 0; // Number of step from endstop of the syringe = position of the syringe.
int positionBottle = 0; // Number of step from endstop of the arm dedicated to bottle = position of the syringe.

byte armState = 0; //arm microswitch is pressed.

Minitel minitel;


void setup() 
{                
  pinMode(PIN_STEP_CAROUSEL, OUTPUT);
  pinMode(PIN_DIR_CAROUSEL, OUTPUT); 
  pinMode(PIN_STEP_SYRINGE, OUTPUT);
  pinMode(PIN_DIR_SYRINGE, OUTPUT); 
  pinMode(PIN_STEP_BOTTLE, OUTPUT);
  pinMode(PIN_DIR_BOTTLE, OUTPUT);  
  pinMode(PIN_GYRO, OUTPUT);
  
  pinMode(PIN_CAROUSEL_ENDSTOP, INPUT);  
  pinMode(PIN_SYRINGE_ENDSTOP, INPUT);
  pinMode(PIN_SYRINGE_CONTACT, INPUT);
  pinMode(PIN_SLOT_ARM, INPUT);
  
  
  // Serial for debugging
  Serial.begin(9600);
  //Serial for cupbot
  Serial1.begin(115200);
  //Init random generator
  randomSeed(analogRead(0));
  
  
  minitel.useDefaultColors();
  minitel.refreshSettings();
  minitel.setMaxSpeed(); // FNCT + P and 4
  //initCupBot();
  initSD();
}


void loop() 
{
  //serviceMode(); //uncomment to activate service mode.
  
  // Init of the bordel
  initSyringe();
  initBottle();
  initCarousel();
  loadCocktails();
  
  minitel.clearScreen();
  minitel.rect((byte)219, 9, 6, 24, 5);
  minitel.text("BARBABOT ", 17, 8);
  minitel.blink();
  minitel.text("PULL SLOT ARM TO PLAY.", 10, 16);
  minitel.noBlink();
  minitel.text("by Tetalab (http://tetalab.org)", 9, 24);
  
  armState = digitalRead(PIN_SLOT_ARM);
  while(armState == 0){
    armState = digitalRead(PIN_SLOT_ARM);
  }
  
  int randNumber = rolling();
  
  if(randNumber == 0){//Jackpot case
    //Démo jackpot
    demoJackpot();
    randNumber = createCocktail();
  }
  
  //printToCup("ROBOEXOTICA 2014", false);
  Cocktail cocktail = loadCocktail(randNumber);
  minitel.clearScreen();
  minitel.text("NOW SERVING :", 8,8);
  minitel.text(String(cocktail.name), 10,9);
  minitel.text("by " + cocktail.author, 10, 10);
  

  //Let's do it.
  makeCocktail(cocktail);
  delay(3000);
}

/*
* Serve a cocktail.
*/
void makeCocktail(Cocktail &cocktail) 
{
  DEBUG(String("begin make cocktail taille ingredient =") + String(sizeof(cocktail.ingredient[0]), DEC));
  
  //Appel cupbot. 
  
  //Put each ingredient.
  for(int i=0; i<sizeof(cocktail.ingredient[0]); i++){
    if(cocktail.ingredient[i][0] != 0){
      minitel.text(String("Serving ") + String(cocktail.ingredient[i][1])+ String("ml of ")+ String(INGREDIENTS[cocktail.ingredient[i][0]]), 1 ,12+i);
      
      int slot = findAlcohol(cocktail.ingredient[i][0], cocktail.ingredient[i][1]);
      if(slot != -1){
        goToCarousel(slot);
        serveAlcohol(slot, cocktail.ingredient[i][1]);
      }else{
         minitel.text("Empty ingredient.Cannot continue.", 1, 14+i);
      }
    }
  }
  minitel.blink();
  minitel.text("DONE! ENJOY!", 17 ,4);
  minitel.noBlink();
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
    digitalWrite(PIN_DIR_CAROUSEL, LOW);
    deltaStep = numberOfStepFromBegin - positionCarousel;
  }else{
    digitalWrite(PIN_DIR_CAROUSEL, HIGH);
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
void serveAlcohol(byte slot, byte quantity) 
{
  DEBUG(String("begin serveAlcohol slot=") + String(slot, DEC));
  
  int numberOfStepFromBegin = 0;
  //If it's a syringe.
  if(slot<23){
    
    //Move until syringe switch
    digitalWrite(PIN_DIR_SYRINGE, HIGH);
    byte syringeContactStopState = digitalRead(PIN_SYRINGE_CONTACT);
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
    //In case of bottle (1 push of bottle = 40ml)
    for(int i = 0; i<(quantity/40);i++){
      digitalWrite(PIN_DIR_BOTTLE, LOW);
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
* Rolling. Choose randomly the right cocktail
* @return void
*/
int rolling() 
{
  DEBUG("demoRolling - debut");
  
  String cocktail1 = "";
  String cocktail2 = "";
  String cocktail3 = "";
  long previousMillis = 0; 
  
  //Random cocktail
  int randNumber = random(NUMBER_OF_COCKTAILS);
  //Si jackpot 1 chance sur 2 de l'avoir).
  /*if(randNumber == 0){
    randNumber = random(2);
    if(randNumber != 0){
      randNumber = random(1, NUMBER_OF_COCKTAILS);
    }
  }*/
  //randNumber = 0;
  
  minitel.clearScreen();
  minitel.text("Rolling!", 4, 8);
  minitel.rect((byte) 219, 4, 10, 30, 7);
  int cocktail = randNumber;
  int nbLoop = (NUMBER_OF_COCKTAILS * 4) + 3;
  int bip = 0;
  for(int i=0;i<nbLoop;i++)
  {
    //cocktail = NUMBER_OF_COCKTAILS - i >0 ? NUMBER_OF_COCKTAILS - i : 0;
    
    for(int j=5;j>0;j--)
    {
        String name = liste_cocktails[cocktail];
        for(int x=0;x<30 - name.length();x++){
          name = name + " ";
        }
        
        if(i == nbLoop - 1 && j == 3) minitel.blink();
        minitel.text(name, 5,10+j);
        
        if(i == nbLoop - 1 && j == 3) minitel.noBlink();
        cocktail = cocktail + 1 >= NUMBER_OF_COCKTAILS ? 0 : cocktail+1;
    }

    //minitel.bip();
       
    //delay for rolling
    unsigned long currentMillis = millis();
    while(currentMillis - previousMillis <= (i+1)*50) {
      currentMillis = millis();
    }
    previousMillis = currentMillis;   
    
  }
  //minitel.bip();
  delay(500);
  //minitel.bip();
  delay(500);
  //minitel.bip();
  delay(2000);
  
  DEBUG("demoRolling - fin");
  return randNumber;
}

/*
* Démo jackpot
* @return void
*/
int demoJackpot() 
{
  DEBUG("demoJackpot - debut");
  //digitalWrite(PIN_GYRO, 255);
  minitel.graphicMode();
  minitel.clearScreen();
  minitel.graphicMode();
  
  bg();
  rainbow(2, 10);  
  body(12, 9);

  drawStar1(28, 4);
  drawStar1(20, 21);

  drawStar2(5, 5, false);
  drawStar2(22, 9, true);
  drawStar2(35, 20, false);
  drawStar2(20, 5, true);

  drawStar2(5, 20, true);
  drawStar3(32, 11);  
  drawFrame();
  minitel.textMode();
  minitel.bgColor(BLACK);
  minitel.text("IT'S JACKPOT TIME!", 8,1);
  delay(5000);
  digitalWrite(PIN_GYRO, 0);
  minitel.clearScreen();
  minitel.text("You win the right to create a cocktail", 1,10);
  minitel.text("Leave your name in the History of cocktail", 1, 11);
  delay(4000);
  
  
  DEBUG("demoJackpot - fin");
}

/*
* Function to describe the cocktail
* @return void
*/
int createCocktail() 
{
  DEBUG("createCocktail - debut");
  String keyChar = "^";
  int remainingQty = 200;
  boolean bEndStep1 = false;
  String cocktailName = "";  
  String author = "";  
  int cocktailNumber = 0;
  int incrementIng = 0;
  Cocktail newCocktail;
  
  minitel.clearScreen();
  minitel.text("Choose ingredients. 10 ingredient max", 1, 1);
  minitel.text("Total maximal amount is 200ml", 1,2);
  minitel.text("Qty from bottle is 40,80,120,160,200ml", 1, 3);
  minitel.text("Max qty from syringe is 50ml. ", 1, 4);
  minitel.text("Press ENVOI to validate each step. ", 1, 5);
  minitel.blink();
  minitel.text("!PRESS ANY KEY TO START!", 9, 7);
  minitel.noBlink();
  
  keyChar = minitel.getKey2();
  while (keyChar == ""){
    keyChar = minitel.getKey2(); 
  }
  minitel.textMode();
  minitel.cursor();
  
  while(remainingQty>0 && !bEndStep1){
    
    String nbIngredient = "";  
    String quantity = "";  
    boolean bValidate = false;
    
    minitel.clearLine(7);
    minitel.text("REMAINING QUANTITY : " + String(remainingQty, DEC)+"ml" , 9, 7);
  
    //Ingredient number
    while(!bValidate){
      nbIngredient = "";
      minitel.clearLine(9);
      minitel.text(String("Ingredient number:                     "), 1, 9);
      minitel.clearLine(10);
      minitel.moveCursorTo(19, 9);
      
      keyChar = minitel.getKey2();
      while (keyChar != "ENVOI"){
         nbIngredient += keyChar;
         keyChar = minitel.getKey2();
      }
      
      //Validation et Confirmation saisie.
      if(nbIngredient.toInt() >= 1  && nbIngredient.toInt() <= 13){//OK
        if(nbIngredient.toInt() >= 1 && nbIngredient.toInt()<=4 && remainingQty<=40){
          //minitel.blink();
          minitel.clearLine(9);
          minitel.clearLine(10);
          minitel.text(String("Not enough remaining quantity for ingredient in bottle."), 1, 9);
          //minitel.noBlink();
          delay(3000);
          nbIngredient = "";
          bValidate = false;
        }else{
          minitel.clearLine(9);
          minitel.clearLine(10);
          minitel.text("You choose : "+ String(INGREDIENTS[nbIngredient.toInt()]) +". Correct? (y/n): ", 1, 9);
          keyChar = minitel.getKey2(); 
          while (keyChar != "Y" && keyChar !="N"){
              keyChar = minitel.getKey2(); 
          }
          
          bValidate = keyChar=="Y" ? true : false;
        }
      }else{
        //minitel.blink();
        minitel.clearLine(9);
        minitel.clearLine(10);
        minitel.text("INCORRECT!!", 1, 9);
        minitel.text("Must be a number between 1 and 13", 1, 10);
        //minitel.noBlink();
        delay(3000);
        nbIngredient = "";
        bValidate = false;
      }        
    }
    
    //QUANTITY
    bValidate = false;
    while(!bValidate){
      quantity = "";
      minitel.clearLine(9);
      minitel.clearLine(10);
      if(nbIngredient.toInt()<= 4){ //Ingredient in bottle.
        minitel.text("Qty (40, 80, 120 or 160ml):             ", 1, 9);
        minitel.moveCursorTo(28, 9);
      }else{
        minitel.text("Qty between 1 and 50ml:                ", 1, 9);
        minitel.moveCursorTo(24, 9);
      }
      
      keyChar = minitel.getKey2(); 
      while (keyChar != "ENVOI"){
        quantity += keyChar;
        keyChar = minitel.getKey2();
      }
      
      DEBUG(String("quantity=")+String(quantity));
      //validation saisie.
      if(nbIngredient.toInt()>4){//Syringe case
        if(quantity.toInt() >= 1  && quantity.toInt() <= 50){//OK
          bValidate = true;
        }else{
          //minitel.blink();
          minitel.clearLine(9);
          minitel.clearLine(10);
          minitel.text("INCORRECT!! Must be between 1 and 50", 1, 9);
          //minitel.noBlink();
          delay(3000);
          bValidate = false;
        }      
      }else{
        if(quantity.toInt() == 40  || quantity.toInt() == 80 || quantity.toInt() == 120 || quantity.toInt() == 160 ){//OK
            bValidate = true;
        }else{
            //minitel.blink();
            minitel.clearLine(9);
            minitel.clearLine(10);
            minitel.text("INCORRECT!! Only 40,80,120 or 160ml are valid", 1, 9);
            //minitel.noBlink();
            delay(3000);
            bValidate = false;
        }      
      } 
     
      //Check remaining quantity
      if(bValidate){
        if(remainingQty - quantity.toInt() < 0 ){
            //minitel.blink();
            minitel.clearLine(9);
            minitel.clearLine(10);
            minitel.text("Not enough remaining quantity! try again!", 1, 9);
            //minitel.noBlink();
            delay(3000);
            bValidate = false;
        }
      }
    }
    
    //ALL is good. Add ingredient.
    remainingQty = remainingQty - quantity.toInt();
    newCocktail.ingredient[incrementIng][0]=nbIngredient.toInt(); 
    newCocktail.ingredient[incrementIng][1]=quantity.toInt();
    
    minitel.clearLine(7);
    minitel.text("REMAINING QUANTITY : " + String(remainingQty, DEC)+"ml" , 9, 7);
    
    if(remainingQty > 0){
      minitel.clearLine(9);
      minitel.clearLine(10);
      minitel.text("Ingredient added succesfully            ", 1, 9);
      minitel.text("Add another ingredient (y/n):          ", 1, 10);
      minitel.moveCursorTo(30, 10);
      keyChar = minitel.getKey2(); 
      while (keyChar != "Y" && keyChar !="N"){
          keyChar = minitel.getKey2(); 
      }
      
      bEndStep1 = keyChar=="N" ? true : false;
    }else{
      bEndStep1 = true;
    }
    incrementIng++;
  }
 
  //Init des var des ingrédients.
  for(int i = incrementIng; i<10;i++){
    newCocktail.ingredient[i][0]=NULL;newCocktail.ingredient[i][1]=NULL;
  }
  
  //Name of the cocktail.
  boolean bEndStep2 = false;
  while(!bEndStep2){
    cocktailName="";
    minitel.clearLine(9);
    minitel.clearLine(10);
    minitel.clearLine(11);
    minitel.text("Name of this cocktail (max 30car):     ", 1, 9);
    minitel.clearLine(10);
    minitel.moveCursorTo(35, 9);
    
    keyChar = minitel.getKey2(); 
    DEBUG(String("keyCharBefore=")+String(keyChar));
    while (keyChar != "ENVOI"){
       cocktailName += keyChar;
       keyChar = minitel.getKey2(); 
    }
    DEBUG(String("cocktailName=")+String(cocktailName));
    newCocktail.name = cocktailName;
    if(cocktailName.length() == 0 || cocktailName.length() > 30){
        //minitel.blink();
        minitel.clearLine(9);
        minitel.clearLine(10);
        minitel.text("Incorrect size for name", 1, 9);
        //minitel.noBlink();
        delay(3000);
        bEndStep2 = false;
      }else{
        minitel.text("\""+ cocktailName + "\" it's correct? (y/n):", 1, 10);
        keyChar = minitel.getKey2(); 
        while (keyChar != "Y" && keyChar !="N"){
            keyChar = minitel.getKey2(); 
        }
        
        bEndStep2 = keyChar=="Y" ? true : false;
      }
  }
  
  //Author
  boolean bEndStep3 = false;
  while(!bEndStep3){
    author = "";
    minitel.clearLine(9);
    minitel.clearLine(10);
    minitel.clearLine(11);
    minitel.text("Your name (max 30car):                 ", 1, 9);
    minitel.clearLine(10);
    minitel.moveCursorTo(23, 9);
    
    keyChar = minitel.getKey2(); 
    while (keyChar != "ENVOI"){
       author += keyChar;
       keyChar = minitel.getKey2(); 
    }
    
    if(author.length() == 0 || author.length() > 30){
      //minitel.blink();
      minitel.clearLine(9);
      minitel.clearLine(10);
      minitel.text("Incorrect size for author", 1, 9);
      //minitel.noBlink();
      delay(3000);
      bEndStep3 = false;
    }else{
      minitel.text("\""+ author + "\" it's correct? (y/n):", 1, 10);
      keyChar = minitel.getKey2(); 
      while (keyChar != "Y" && keyChar !="N"){
          keyChar = minitel.getKey2(); 
      }
      
      bEndStep3 = keyChar=="Y" ? true : false;
    }
  }
      
  DEBUG(String("author=")+String(author));
  newCocktail.author = author;
  
  minitel.clearLine(9);
  minitel.clearLine(10);
  minitel.text("Done! Let's taste your creation!       ", 1, 9);
  minitel.noCursor();
  
  saveNewCocktail(newCocktail);
  
  delay(2000);
  return NUMBER_OF_COCKTAILS - 1 ;
  DEBUG("createCocktail - fin");
}

/*
* Carousel go to 0.
*/
int initCarousel() 
{
  int nbStep = 0;
  DEBUG("begin init carousel");
  
  //While endstop not reach.
  digitalWrite(PIN_DIR_CAROUSEL, HIGH);
  byte carouselEndstopState = digitalRead(PIN_CAROUSEL_ENDSTOP);
  while(carouselEndstopState == HIGH){
    //Move on
    digitalWrite(PIN_STEP_CAROUSEL, HIGH);   
    delay(SPEED_CAROUSEL);
    digitalWrite(PIN_STEP_CAROUSEL, LOW);
    //Read value of endstop. 
    carouselEndstopState = digitalRead(PIN_CAROUSEL_ENDSTOP);
    nbStep++;
  }
  
  //Un petit coup doucement pour le rebondi
  /*delay(1000);
  carouselEndstopState = digitalRead(PIN_CAROUSEL_ENDSTOP);
  while(carouselEndstopState == 1){
    //Move on
    digitalWrite(PIN_STEP_CAROUSEL, HIGH);   
    delay(100);
    digitalWrite(PIN_STEP_CAROUSEL, LOW);
    //Read value of endstop. 
    carouselEndstopState = digitalRead(PIN_CAROUSEL_ENDSTOP);
  }*/
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
  digitalWrite(PIN_DIR_SYRINGE, LOW);
  byte syringeEndstopState = digitalRead(PIN_SYRINGE_ENDSTOP);
  while(syringeEndstopState == HIGH){ //reversed microswitch
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
  digitalWrite(PIN_DIR_BOTTLE, HIGH);
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

void drawFrame() {
  minitel.graphicMode();
  minitel.rect((byte) minitel.getGraphicChar("011001"), 1, 1, 39, 24);
 
}


void drawStar1(int x, int y) {
  minitel.graphicMode();
  minitel.textColor(WHITE);
  minitel.blink();
  minitel.graphic("111111", x, y);
  minitel.noBlink();
  minitel.graphic("111111", x, y-1);
  minitel.graphic("111111", x, y+1);
  minitel.graphic("111111", x-1, y);
  minitel.graphic("111111", x+1, y);  
}

void drawStar2(int x, int y, boolean b) {
  minitel.textMode();
  //minitel.minitel.bgColor
  minitel.bgColor(RED);
  if (b) {
    minitel.blink();
  }
  minitel.text(" *", x, y);
  if (b) {
    minitel.noBlink();
  }
}

void drawStar3(int x, int y) {
  minitel.graphicMode();
  minitel.bgColor(RED);
  minitel.textColor(WHITE);

  minitel.graphic("001111", x, y-2);  
  minitel.graphic("111100", x, y-1);  
  minitel.graphic("111100", x, y);
  minitel.graphic("111111", x, y+1);  
  minitel.graphic("110000", x, y+2);  


  minitel.graphic("101000", x-1, y);  
  minitel.graphic("111100", x-2, y);  

  minitel.graphic("010100", x+1, y);  
  minitel.graphic("111100", x+2, y);  


}



void body(int x, int y) {
  minitel.graphicMode();

  minitel.moveCursorTo(x, y);
  minitel.bgColor(RED);
  minitel.textColor(BLACK);

  minitel.graphic("000001");
  minitel.graphic("000011");
  minitel.repeat(7);

  for (int i=1; i<7; i++) {
    minitel.moveCursorTo(x, y+i);
    minitel.graphic("101010");
    minitel.moveCursor(RIGHT, 8);
    minitel.graphic("101010");
  }


  minitel.moveCursorTo(x-1, y+7);
  minitel.graphic("011011");  


  // Legs
  minitel.moveCursorTo(x, y+7);
  minitel.graphic("111000");
  minitel.graphic("111001");  
  minitel.graphic("111010");

  minitel.graphic("110000");
  minitel.graphic("110000");
  minitel.graphic("111001");
  minitel.graphic("111010");

  minitel.graphic("110100");
  minitel.graphic("110111");

  // Fur
  minitel.moveCursorTo(x+2, y+2);
  minitel.graphic("010000");
  minitel.moveCursorTo(x+4, y+3);
  minitel.graphic("010010");

  minitel.moveCursorTo(x+2, y+4);
  minitel.graphic("010010");
  minitel.moveCursorTo(x+4, y+5);
  minitel.graphic("010010");

  minitel.moveCursorTo(x+6, y+2);
  minitel.graphic("100001");

  // tail
  minitel.moveCursorTo(x-3, y+4);
  minitel.graphic("111011");
  minitel.graphic("110011");
  minitel.graphic("110011");

  // Head
  minitel.moveCursorTo(x+6, y+3);
  minitel.graphic("011010");
  minitel.graphic("001001");
  minitel.graphic("000011");
  minitel.graphic("100110");
  minitel.graphic("100101");
  minitel.graphic("000000");

  minitel.moveCursorTo(x+5, y+4);
  minitel.graphic("000001");
  minitel.graphic("101000");
  minitel.graphic("000010");
  minitel.graphic("000000");
  minitel.graphic("000001");
  minitel.graphic("010100");
  minitel.graphic("000010");

  minitel.moveCursorTo(x+5, y+5);
  minitel.graphic("010101");
  minitel.graphic("010000");
  minitel.graphic("100010"); // eyeleft
  minitel.graphic("100010");
  minitel.graphic("110010");
  minitel.graphic("000000");
  minitel.graphic("101010");

  minitel.moveCursorTo(x+6, y+6);
  minitel.graphic("101001");
  minitel.graphic("110000");
  minitel.graphic("110000");
  minitel.graphic("100000");
  minitel.graphic("010110");

  minitel.moveCursorTo(x+9, y+7);
  minitel.graphic("110000");
  minitel.graphic("100000");
}

void bg() {
  minitel.graphicMode();
  minitel.textColor(RED);
  minitel.moveCursorTo(1, 1);
  for (int i=1; i<25; i++) {
    minitel.graphic("111111");
    minitel.repeat(39);
  }

}

void rainbow(int x, int y) {

  //WHITE, YELLOW, CYAN, GREEN, BLUE, RED, MAGENTA, BLACK
  minitel.graphicMode();

  int colors[] = {
    RED, MAGENTA, YELLOW, WHITE, GREEN, CYAN, BLUE, RED  };

  for (int i=0; i<8; i++) {
    minitel.bgColor(colors[i]);
    minitel.textColor(colors[i+1]);
    minitel.graphic("001111", x, y+i);
    //    minitel.repeat(1);
    minitel.graphic("111111");
    minitel.repeat(2);
    minitel.graphic("001111");
    minitel.repeat(2);
    minitel.graphic("111111");
    minitel.repeat(2);
  }
}
