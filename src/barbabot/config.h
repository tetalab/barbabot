//#include<Arduino.h>
#include "Minitel.h"

/** PINS and CONSTANTS **/
int nbStepForRevolutionHastaSiempreCommandante = 800;

//DEBUG MODE on Serial
const bool DEBUG_MODE = true;

//CAROUSEL
#define STEP_PER_REVOLUTION 200 //Default number of steps by revolution for the stepper.
#define PIN_DIR_CAROUSEL 24   //Pin direction carousel
#define PIN_STEP_CAROUSEL 25  //Pin step carousel
#define PIN_CAROUSEL_ENDSTOP 7 //yellow wire
#define HIGH_CAROUSEL LOW
#define LOW_CAROUSEL HIGH
#define SPEED_CAROUSEL 35 //delay between each step.

//SYRINGE
#define PIN_STEP_SYRINGE 27  //Pin step syringe
#define PIN_DIR_SYRINGE 26   //Pin direction syringe
#define PIN_SYRINGE_ENDSTOP 5 // brown wire
#define HIGH_SYRINGE HIGH
#define LOW_SYRINGE LOW
#define PIN_SYRINGE_CONTACT 6 //red wire
#define STEP_TO_ML 160 //Number of step for 1 ml 10
#define MAX_STEP_SYRINGE 16000 // maximum step to push a syringe until it destroy everything.
#define SPEED_SYRINGE 3 //delay between each step.
#define STEP_TO_SYRINGE 34 // Number of step to reach a syringe.

//BOTTLE
#define PIN_STEP_BOTTLE 29  //Pin step bottle
#define PIN_DIR_BOTTLE 28   //Pin direction bottle
#define HIGH_BOTTLE LOW
#define LOW_BOTTLE HIGH
#define STEP_TO_PRESS_BOTTLE 4000 // Number of step to have 40ml from bottle.
#define MAX_STEP_BOTTLE 0 // maximum step to push the arm of the dispenser.
#define SPEED_BOTTLE 3 //delay between each step.
#define STEP_TO_BOTTLE 210 // Number of step between each bottle.

//ARM
#define PIN_SLOT_ARM 12 //red wire

//DIVERS
#define MAX_VERRE_ML 200 //Qt� maximale d'un verre.

/** COCKTAILS **/

// Defining a cocktail
typedef struct{
    String name;
    String author;
    byte ingredient[10][2];//[num�ro ingr�dient][quantit�]
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
const char* INGREDIENTS[14] = {"vide", "Tequila", "Vodka", "Rhum", "Cointreau", "Orange juice", "Curacao", "Liquid cane sugar", "Lime juice", "Love", "Cranberry juice", "Bailey", "Absinthe", "Perrier"};

const int SIZEOF_BAR = 27;

// Composition du carousel avec qté restante.
byte BAR[SIZEOF_BAR][2]={
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
  {9, 100}, // Love
  {0, 100},
  {0, 100},
  {0, 100},
  {0, 100},
  {0, 100},
  {0, 100},
  {0, 100},
  {2, 750}, // 22 Vodka
  {1, 750}, // 23 Tequila
  {3, 750}, // 24 Rhum
  {13, 750} // 25 Perrier
};

//QueueArray <Cocktail> liste_cocktails;

Cocktail liste_cocktails[50]; //Array of cocktail. 

int NUMBER_OF_COCKTAILS = 0; // Number of cocktails in the list
