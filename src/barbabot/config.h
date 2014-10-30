#include<Arduino.h>

/** PINS and CONSTANTS **/
int nbStepForRevolutionHastaSiempreCommandante = 750;

//DEBUG MODE 
const bool DEBUG_MODE = true;

//CAROUSEL
const int STEP_PER_REVOLUTION = 200; //Default number of steps by revolution for the stepper.
const int PIN_DIR_CAROUSEL = 24;   //Pin direction carousel
const int PIN_STEP_CAROUSEL = 25;  //Pin step carousel
const int PIN_CAROUSEL_ENDSTOP = 7;
const int HIGH_CAROUSEL = LOW;
const int LOW_CAROUSEL = HIGH;
const int SPEED_CAROUSEL = 40; //delay between each step.

//SYRINGE
const int PIN_STEP_SYRINGE = 27;  //Pin step syringe
const int PIN_DIR_SYRINGE = 26;   //Pin direction syringe
const int PIN_SYRINGE_ENDSTOP = 5;
const int HIGH_SYRINGE = HIGH;
const int LOW_SYRINGE = LOW;
const int PIN_SYRINGE_CONTACT = 6;
const int STEP_TO_ML = 2; //Number of step for 1 ml
const int MAX_STEP_SYRINGE = 0; // maximum step to push a syringe until it destroy everything.
const int SPEED_SYRINGE = 5; //delay between each step.
int STEP_TO_SYRINGE = nbStepForRevolutionHastaSiempreCommandante/23; // Number of step to reach a syringe.

//BOTTLE
const int PIN_STEP_BOTTLE = 17;  //Pin step bottle
const int PIN_DIR_BOTTLE = 16;   //Pin direction bottle
const int PIN_BOTTLE_ENDSTOP = 21;
const int HIGH_BOTTLE = HIGH;
const int LOW_BOTTLE = LOW;
const int STEP_TO_PRESS_BOTTLE = 20; // Number of step to have 40ml from bottle.
const int MAX_STEP_BOTTLE = 0; // maximum step to push the arm of the dispenser.
const int SPEED_BOTTLE = 10; //delay between each step.
int STEP_TO_BOTTLE = nbStepForRevolutionHastaSiempreCommandante/20; // Number of step between each bottle.

//ARM
const int PIN_SLOT_ARM = 12;

//DIVERS
const int MAX_VERRE_ML = 200; //Qt� maximale d'un verre.

/** COCKTAILS **/

// Defining a cocktail
typedef struct{
    String name;
    String author;
    int avis; //avis de 0 a 5
    int ingredient[10][2];//[num�ro ingr�dient][quantit�]
}Cocktail;

/* Liste des ingrédients
* 0 = vide
* 1 = vide
* 2 = vodka (Bottle)
* 3 = rhum (Bottle)
* 4 = cointreau
* 5 = jus orange
* 6 = curacao
* 7 = sucre de canne liquide
* 8 = jus de citron
* 9 = Tequila (Bottle)
* 10 = jus de cranberry
* 11 = Bailey
* 12 = Absinthe
* 13 = Perrier
*/
const char* INGREDIENTS[14] = {"vide", "vide", "vodka", "rhum", "Cointreau", "Orange juice", "Curacao", "Liquid cane sugar", "Lime juice", "Tequila", "Cranberry juice", "Bailey", "Absinthe", "Perrier"};

const int SIZEOF_BAR = 27;

// Composition du carousel avec qté restante.
int BAR[SIZEOF_BAR][2]={
  {8, 100}, // jus de citron
  {8, 100}, // jus de citron
  {4, 100}, // cointreau
  {4, 100}, // cointreau
  {6, 100}, // curacao
  {6, 100}, // curacao
  {10, 100}, // jus de cranberry
  {10, 100}, // jus de cranberry
  {12, 100}, // Absinthe
  {12, 100}, // Absinthe
  {11, 100}, // Bailey
  {11, 100}, // Bailey
  {7, 100}, //Sucre de canne liquide
  {5, 100}, // Jus d'orange
  {5, 100}, // Jus d'orange
  {0, 100},
  {0, 100},
  {0, 100},
  {0, 100},
  {0, 100},
  {0, 100},
  {0, 100},
  {0, 100},
  {2, 750}, // Vodka
  {3, 750}, // Rhum
  {9, 750}, // Tequila
  {13, 750} // Perrier
};

const int NUMBER_OF_COCKTAILS = 1;
Cocktail liste_cocktails[NUMBER_OF_COCKTAILS]; //increase with 

void initListeCocktails(){
  //Init predefined cocktails.
  //Attention : les alcools en bouteille ne peuvent avoir des dosages que de 40ml, 80ml, 120ml, etc etc
  liste_cocktails[0].name="Margarita";
  liste_cocktails[0].author="Margarita Sames";
  liste_cocktails[0].avis=4;
  liste_cocktails[0].ingredient[0][0]=9; liste_cocktails[0].ingredient[0][1]=40;//Tequila 4cl.
  liste_cocktails[0].ingredient[1][0]=4; liste_cocktails[0].ingredient[1][1]=30;//Cointreau 30ml.
  liste_cocktails[0].ingredient[2][0]=8; liste_cocktails[0].ingredient[2][1]=30;//Jus de citron 20ml.
  
  /*liste_cocktails[1].name="Cosmopolitan";
  liste_cocktails[1].author="Cheryl Cook";
  liste_cocktails[1].avis=4;
  liste_cocktails[1].ingredient[0][0]=2; liste_cocktails[1].ingredient[0][1]=40;//Vodka 4cl.
  liste_cocktails[1].ingredient[1][0]=4; liste_cocktails[1].ingredient[1][1]=20;//Cointreau 30ml.
  liste_cocktails[1].ingredient[2][0]=10; liste_cocktails[1].ingredient[2][1]=20;//Jus de cranberry 20ml.
  liste_cocktails[1].ingredient[3][0]=8; liste_cocktails[1].ingredient[3][1]=10;//Jus de citron 10ml.
  
  liste_cocktails[2].name="Cointreaupolitan";
  liste_cocktails[2].author="Cointreau";
  liste_cocktails[2].avis=3;
  liste_cocktails[2].ingredient[0][0]=4; liste_cocktails[2].ingredient[0][1]=50;//Cointreau 50ml.
  liste_cocktails[2].ingredient[1][0]=10; liste_cocktails[2].ingredient[1][1]=30;//Jus de cranberry 30ml.
  liste_cocktails[2].ingredient[2][0]=8; liste_cocktails[2].ingredient[2][1]=20;//Jus de citron 20ml.
  
  liste_cocktails[3].name="Blue lagoon";
  liste_cocktails[3].author="Andy MacElhone";
  liste_cocktails[3].avis=4;
  liste_cocktails[3].ingredient[0][0]=2; liste_cocktails[3].ingredient[0][1]=40;//Vodka 4cl.
  liste_cocktails[3].ingredient[1][0]=6; liste_cocktails[3].ingredient[1][1]=30;//Curacao 30ml.
  liste_cocktails[3].ingredient[2][0]=8; liste_cocktails[3].ingredient[2][1]=20;//Jus de citron 20ml.
  
  liste_cocktails[4].name="Head Shock";
  liste_cocktails[4].author="Anonymous";
  liste_cocktails[4].avis=4;
  liste_cocktails[4].ingredient[0][0]=9; liste_cocktails[4].ingredient[0][1]=40;//Tequila 4cl (normalement 2 mais bon).
  liste_cocktails[4].ingredient[1][0]=12; liste_cocktails[4].ingredient[1][1]=40;//Absinthe 40ml.
  
  liste_cocktails[5].name="Bob Marley Absinthe";
  liste_cocktails[5].author="Jah";
  liste_cocktails[5].avis=4;
  liste_cocktails[5].ingredient[0][0]=13; liste_cocktails[5].ingredient[0][1]=80;//Perrier 8cl (normalement 6 mais bon).
  liste_cocktails[5].ingredient[1][0]=12; liste_cocktails[5].ingredient[1][1]=30;//Absinthe 30ml.
  liste_cocktails[5].ingredient[2][0]=5; liste_cocktails[5].ingredient[2][1]=30;//Jus d'orange 30ml.
  liste_cocktails[5].ingredient[3][0]=7; liste_cocktails[5].ingredient[3][1]=20;//sucre de canne liquide 20ml.*/
  
  Serial.println(String("COCKTAILS AVALAIBLES :") + String(NUMBER_OF_COCKTAILS, DEC));
}
