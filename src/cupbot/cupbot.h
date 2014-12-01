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
 #include "symboles.h"
 
 int posX = 0;
 int posY = 0;
 
 const byte espaceLettres = 10;
 const byte interligne = 10;
 
 void initCupBot()
 {
   posX = 0;
   posY = 0;
 }
 
 String charToString(char* mot){
   String result = "";
   for(int i=0;i<strlen(mot);i++){
     result = result + mot[i];
   }
   return result;
 }
 
  int strpos(char *haystack, char *needle)
  {
     char *p = strstr(haystack, needle);
     if (p)
        return p - haystack;
     return -1;
  }
  
  char *substring(size_t start, size_t stop, const char *src, char *dst, size_t size)
  {
     int count = stop - start;
     if ( count >= --size )
     {
        count = size;
     }
     sprintf(dst, "%.*s", count, src + start);
     return dst;
  }
 
 char* generateGCodeStream(char lettre, boolean newLine){
   Serial.println("generateGCodeStream - debut");
   Serial.println("LETTRE=" + lettre);
   char* gcode;
   
   if(newLine){
     posY += 10;
   }
   
   //décalage entre les lettres.
   posX += 10;
   
   strcat(gcode, "G0 Z2\n");

   //Recup du symbole.
   for(int j = 0; j < 45; j++){
     boolean bTrouve = false;
     if(lettre == SYMBOLES[j][0][0]){
        //parcours des commandes et calcul des offsets.
        for(int k = 0; k < 15; k++){
          if(SYMBOLES[j][k] != NULL){
            strcat(gcode,"G");
            
            int startX = strpos(SYMBOLES[j][k], "X\0");
            int startY = strpos(SYMBOLES[j][k], "Y\0");
            int startI = strpos(SYMBOLES[j][k], "I\0");
            
            if(startX != -1){//Si mouvement en X et Y
              char x[4];
              substring(startX, startY - 1, SYMBOLES[j][k], x, sizeof x);
              char y[4]="0\0";
              if(startI != -1){
                //y = tmp.substring(startY, startI - 1).toInt();
                substring(startY, startI - 1, SYMBOLES[j][k], y, sizeof y);
              }else{
                //y = tmp.substring(startY).toInt();
                substring(startY, strlen(SYMBOLES[j][k]), SYMBOLES[j][k], y, sizeof y);
              }
              
              //Offset par rapport à la position actuelle de la tête.
              posX += atoi(x);
              posY += atoi(y);
              char debut[10];
              substring(0, startX, SYMBOLES[j][k], debut, sizeof debut);
              strcat(gcode,debut);
              strcat(gcode, itoa(posX, x, 10));
              strcat(gcode, " Y");
              strcat(gcode, itoa(posY, y, 10));
              
              //gcode += substring(0, startX) + String(posX) + " Y"+ String(posY);
              
              if(startI != 1){
                char fin[10];
                substring(startI, strlen(SYMBOLES[j][k]), SYMBOLES[j][k], fin, sizeof fin);
                strcat(gcode,fin);
                //gcode += " " + tmp.substring(0, startI);
              }
              
            }else{//Sinon axe Z et on prend la commande telle quelle
              strcat(gcode,SYMBOLES[j][k]);
            }
            
          }
          
        }
     }else{//Si symbole non trouvé on génére un espace.
       
     }
     
   }
   strcat(gcode,"\n");
   Serial.println("GCODE="+String(gcode));
   Serial.println("generateGCodeStream - fin");
   return gcode;
 }
