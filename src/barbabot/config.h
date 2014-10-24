#include<Arduino.h>




/** PINS and CONSTANTS **/

//CAROUSEL
const int STEP_PER_REVOLUTION = 200; //Default number of steps by revolution for the stepper.
const int PIN_STEP_CAROUSEL = 12;  //Pin step carousel
const int PIN_DIR_CAROUSEL = 13;   //Pin direction carousel
const int PIN_CAROUSEL_ENDSTOP = 18;
const int HIGH_CAROUSEL = HIGH;
const int LOW_CAROUSEL = LOW;
const int SPEED_CAROUSEL = 10; //delay between each step.

//SYRINGE
const int PIN_STEP_SYRINGE = 14;  //Pin step syringe
const int PIN_DIR_SYRINGE = 15;   //Pin direction syringe
const int PIN_SYRINGE_ENDSTOP = 19;
const int HIGH_SYRINGE = HIGH;
const int LOW_SYRINGE = LOW;
const int PIN_SYRINGE_CONTACT = 20;
const int STEP_TO_ML = 2; //Number of step for 1 ml
const int MAX_STEP_SYRINGE = 0; // maximum step to push a syringe until it destroy everything.
const int SPEED_SYRINGE = 10; //delay between each step.
const int STEP_TO_SYRINGE = 8; // Number of step to reach a syringe.

//BOTTLE
const int PIN_STEP_BOTTLE = 16;  //Pin step bottle
const int PIN_DIR_BOTTLE = 17;   //Pin direction bottle
const int PIN_BOTTLE_ENDSTOP = 21;
const int HIGH_BOTTLE = HIGH;
const int LOW_BOTTLE = LOW;
const int STEP_TO_PRESS_BOTTLE = 20; // Number of step to have 4cl from bottle.
const int MAX_STEP_BOTTLE = 0; // maximum step to push the arm of the dispenser.
const int SPEED_BOTTLE = 10; //delay between each step.
const int STEP_TO_BOTTLE = 50; // Number of step between each bottle.

//ARM
const int PIN_SLOT_ARM = 22;

//DIVERS
const int MAX_VERRE_ML = 200; //Qt� maximale d'un verre.

/** COCKTAILS **/

// Defining a cocktail
typedef struct{
    String name;
    String author;
    int avis; //avis de 0 � 5
    int ingredient[10][1];//[num�ro ingr�dient][quantit�]
}Cocktail;

/* Liste des ingrédients dans chacune des 25 seringues + 4 bouteille. Contient la quantité restante.
* 0 = vide
* 2 = vodka (à mettre dans bouteille)
* 3 = rhum (à mettre dans bouteille)
* 4 = cointreau
* 5 = jus orange
* 6 = curacao
* 7 = sucre de canne liquide
* 8 = jus de citron
* 9 = Tequila (à mettre dans bouteille)
*/
int BAR[29][2] = {
  {8,100},
  {8,100},
  {8,100},
  {4,100},
  {4,100},
  {6,100},
  {6,100},
  {7,100},
  {7,100},
  {0,100},
  {0,100},
  {0,100},
  {0,100},
  {0,100},
  {0,100},
  {0,100},
  {0,100},
  {0,100},
  {0,100},
  {0,100},
  {0,100},
  {0,100},
  {0,100},
  {0,100},
  {0,100},
  {2,18}, // ici commence les bouteilles (75cl = 75/4 = 18.75 = 18 doses).
  {3,18},
  {9,18},
  {0,18}
};

