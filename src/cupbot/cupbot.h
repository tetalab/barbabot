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
 
 char* generateGCodeStream(char* mot, boolean newLine){
   Serial1.println("generateGCodeStream - debut");
   String gcode;
   
   if(newLine){
     posY += 10;
   }
   
   for(int i = 0; i < sizeof(mot); i++){
     gcode += "G0 Z2\n";
     
     //décalage entre les lettres.
     if(i==0 && posX>0){
       posX += 10;
     } 
     
     //Recup du symbole.
     for(int j = 0; j < 45; j++){
       boolean bTrouve = false;
       if(mot[i] == SYMBOLES[j][0][0]){
          //parcours des commandes et calcul des offsets.
          for(int k = 0; k < 15; k++){
            if(SYMBOLES[j][k] != NULL){
              gcode += "G";
              String tmp = charToString(SYMBOLES[j][k]);
              int startX = tmp.indexOf('X');
              int startY = tmp.indexOf('Y');
              int startI = tmp.indexOf('I');
              
              if(startX != -1){//Si mouvement en X et Y
                int x = tmp.substring(startX, startY - 1).toInt();
                int y = 0;
                if(tmp.indexOf('I') != -1){
                  y = tmp.substring(startY, startI - 1).toInt();
                }else{
                  y = tmp.substring(startY).toInt();
                }
                
                //Offset par rapport à la position actuelle de la tête.
                posX += x;
                posY += y;
                gcode += tmp.substring(0, startX) + String(posX) + " Y"+ String(posY);
                if(startI != 1){
                  gcode += " " + tmp.substring(0, startI);
                }
                
              }else{//Sinon axe Z et on prend la commande telle quelle
                gcode += charToString(SYMBOLES[j][k]);
              }
              
            }
            
          }
       }else{//Si symbole non trouvé on génére un espace.
         
       }
       
     }
     gcode += "\n";
     Serial1.println("GCODE="+String(gcode));
   }
   
   Serial1.println("generateGCodeStream - fin");
 }
