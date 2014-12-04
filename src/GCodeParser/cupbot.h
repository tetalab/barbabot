/* 
 CUPBOT
 
 Program to draw dick on a plastic cup
 Tested with arduino Mega on Serial2 with other Mega who run modified MakerBlock XY Plotter firmware (modified to run on Serial2).
 Created 27 November 2014 : V.0
 by Flo Gales.
 
 License Rien à branler.
 License Do What The Fuck You Want.
 */
#include <Arduino.h>
//#include "symboles.h"

double posX = 0;
double posY = 0;

const byte espaceLettres = 10;
const byte interligne = 10;

// Defining a gcode for a symbol
typedef struct{
  char car;
  char gcode[16][30];
}
Symbole;

void initCupBot()
{
  posX = 0;
  posY = 0;
}

int strpos(char *haystack, char *needle)
{
  char *p = strstr(haystack, needle);
  if (p)
    return p - haystack;
  return -1;
}

char * substr(char *chaineSource,int pos,int len) { 
  // Retourne la sous-chaine de la chaine chaineSource 
  // depuis la position pos sur la longueur len

    char * dest=NULL;                        
  if (len>0) {                  
    /* allocation et mise à zéro */
    dest = (char *)calloc(len+1, 1);      
    /* vérification de la réussite de l'allocation*/
    if(NULL != dest) {
      strncat(dest,chaineSource+pos,len);            
    }
  }                                       
  return dest;                            
}

/*
* Retrieve from SD file the list of cocktails.
 */
void readCurrentLineToSymbole(Symbole &symbole, int i) 
{
  //Serial.println("readCurrentLineToCocktail - debut");

  //char car = myFile.read();
  //String ligneCSV;

  //while(car != '\n' && car != -1){
    //ligneCSV = symboles[i][0];
    //car = myFile.read(); 
  //}

  //int posComma = 0;
    //posComma = ligneCSV.indexOf(',');
    symbole.car = SYMBOLES[i][0][0];
    //ligneCSV = ligneCSV.substring(posComma+1);
    //Serial.println(symbole.car); 
    for(int j = 1; j<16; j++){
      memset(symbole.gcode[j], 0, 30);
      if(strlen(SYMBOLES[i][j])>1){
        strcpy(symbole.gcode[j], SYMBOLES[i][j]);
      }
    }

  //Serial.println("car="+String(car, DEC));
  //Serial.println("readCurrentLineToCocktail - fin");
}

/*
* Retrieve from SD file the list of cocktails.
 */
Symbole loadSymbole(char car)
{
  //Serial.println("loadCocktails - debut");
  /*if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
  }
  if (!SD.exists("symboles.txt")) {
    Serial.println("symboles.txt doesn't exist.");
  }
  File myFile = SD.open("symboles.txt", FILE_READ);*/

  Symbole symbole;
  boolean bEndOfFile = false;
  boolean bTrouve = false;
  int i = 0;
  while(!bTrouve && i<37){
    readCurrentLineToSymbole(symbole, i);
    if(car == symbole.car){
      bTrouve = true;
      i = 37;
    }else{
      i++;
    }
  }

  if(!bTrouve){ //gcode space by default
    //symbole.car=' ';
    //strcpy(symbole.gcode[0],"G0 X0 Y0");
    //Serial.println("symbole non trouvé");
  }
  else{
    //Serial.println("symbole trouvé");
    /*for(int i = 0;i<16 ;i++){
      Serial.println(symbole.gcode[i]);
    }*/
  }

  //myFile.close();

  //Serial.println("loadCocktails - fin");
  return symbole;
}

void generateGCodeStream(char lettre, boolean newLine){
  //Serial.println("generateGCodeStream - debut");
  //Serial.println("LETTRE=");
  //Serial.println(lettre);
  Symbole symbole;
  //hauteur de ligne
  if(newLine){
    posY += 10;
  }

  //décalage entre les lettres.
  posX += 20;

  //Pen up (down commande but servo inverted)
  process_string("G0 Z-1", strlen("G0 Z0"));
  //clear command.
  init_process_string();
      
  symbole = loadSymbole(lettre);

  //parcours des commandes et calcul des offsets.
  for(int k = 0; k < 16; k++){
    if(strlen(symbole.gcode[k]) > 1){
      char gcode[128];
      memset(gcode, 0, 128);
      //strcat(symbole.gcode[k],"G");
      int startX = strpos(symbole.gcode[k], "X\0");
      int startY = strpos(symbole.gcode[k], "Y\0");
      int startI = strpos(symbole.gcode[k], "I\0");
      
      if(startX != -1){//Si mouvement en X et Y

        char* x = substr(symbole.gcode[k], startX+1, (startY - 1) - (startX+1));
        char* y;
        if(startI != -1){
          //y = tmp.substring(startY, startI - 1).toInt();
          y = substr(symbole.gcode[k], startY+1, (startI - 1) - (startY+1));
        }
        else{
          //y = tmp.substring(startY).toInt();
          //y = substring(symbole.gcode[k], startY +1, strlen(symbole.gcode[k]));
          y = substr(symbole.gcode[k], startY+1, strlen(symbole.gcode[k]) - (startY+1));
          //substring(startY, strlen(symbole.gcode[k]), symbole.gcode[k], y, sizeof y);
        }
        //Serial.println(x);
        //Serial.println(y);
        //Offset par rapport à la position actuelle de la tête.
        double decalageX = posX + atof(x);
        double decalageY = posY + atof(y);
        char* debut = substr(symbole.gcode[k], 0, startX+1);
        strcat(gcode, debut);
        strcat(gcode, dtostrf(decalageX, 0, 3, x));
        strcat(gcode, " Y");
        strcat(gcode, dtostrf(decalageY, 0, 3, y));
        strcat(gcode, " ");

        //gcode += substring(0, startX) + String(posX) + " Y"+ String(posY);
        if(startI != -1){
          char* fin = substr(symbole.gcode[k], startI, strlen(symbole.gcode[k]) - startI);
          strcat(gcode,fin);
          //gcode += " " + tmp.substring(0, startI);
        }

      }
      else{//Sinon axe Z et on prend la commande telle quelle
        strcat(gcode, symbole.gcode[k]);
      }

      //strcat(gcode,"\n");
      
      
      /*Serial.println("EXECUTION DE=");
      Serial.println(gcode);*/
      process_string(gcode, strlen(gcode));
      //clear command.
      init_process_string();
      //delay(2000);
      
      //Serial1.println(gcode);
      

    }
  }
  //Serial.println("generateGCodeStream - fin");
 }


void generateAndSendGCode(char* phrase){
  boolean bNewLine = true;
  for(int i = 0;i < strlen(phrase); i++){
    generateGCodeStream(phrase[i], bNewLine);
    bNewLine = false;
  }
  /*Serial1.println("G0 Z2");
   Serial1.println("G0 XO YO");
   Serial1.println("G0 X50 YO");
   Serial1.println("G0 X50 Y50");*/
}

