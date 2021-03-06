/* 
 SAUVEGARDE BARBABOT
 
 Created 29 November 2014 : V.0
 by Flo Gales.

 License Rien à branler.
 License Do What The Fuck You Want.
 */


 const int chipSelect = 53;
 
 void initSD()
 {
   Serial.println("openSD - debut");
   pinMode(10, OUTPUT);
  
  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    return;
  }
  File myFile;
  if (SD.exists("barbabot.txt")) {
    Serial.println("barbabot.txt exists");
    //SD.remove("barbabot.txt");
  }
  else {
    Serial.println("barbabot.txt doesn't exist.");
    Serial.println("Creating barbabot.txt...");
    myFile = SD.open("barbabot.txt", FILE_WRITE);
    myFile.close();
  }
  
  Serial.println("openSD - fin");
 }
 
/*
* Save a new cocktail to SD file.
*/
void saveNewCocktail(Cocktail newCocktail) 
{
  //Serial.println("saveNewCocktail - debut");
  File myFile;
  myFile = SD.open("barbabot.txt", FILE_WRITE);
  newCocktail.name.toUpperCase();
  newCocktail.author.toUpperCase();
  String ligne = newCocktail.name + "," + newCocktail.author;
  
  for(int i = 0 ; i<10;i++){
    ligne += ","+ String(newCocktail.ingredient[i][0], DEC) +","+ String(newCocktail.ingredient[i][1], DEC);
  }
  ligne = ligne + "\n";
  
  myFile.println(ligne);
  myFile.close();
  char* name;
  newCocktail.name.toCharArray(name, newCocktail.name.length());
  strcpy(liste_cocktails[NUMBER_OF_COCKTAILS], name);
  NUMBER_OF_COCKTAILS++;
  
  //Serial.println("saveNewCocktail - fin");
}

/*
* Retrieve from SD file the list of cocktails.
*/
boolean readCurrentLineToCocktail(Cocktail &cocktail, File myFile) 
{
  Serial.println("readCurrentLineToCocktail - debut");
  
  char car = myFile.read();
  String ligneCSV;
  
  cocktail.name="";
  cocktail.author="";
  
  while(car != '\n' && car != -1){
    ligneCSV += car;
    car = myFile.read(); 
  }
 
 if(ligneCSV.length() >1){
    int posComma = 0;
    posComma = ligneCSV.indexOf(',');
    cocktail.name = ligneCSV.substring(0, posComma);
    ligneCSV = ligneCSV.substring(posComma+1);
    
    posComma = ligneCSV.indexOf(',');
    cocktail.author = ligneCSV.substring(0, posComma);
    ligneCSV = ligneCSV.substring(posComma+1);
    
    for(int i = 0; i<10; i++){
      posComma = ligneCSV.indexOf(',');
      cocktail.ingredient[i][0] = ligneCSV.substring(0, posComma).toInt();
      ligneCSV = ligneCSV.substring(posComma+1);
      
      posComma = ligneCSV.indexOf(',');
      cocktail.ingredient[i][1] = ligneCSV.substring(0, posComma).toInt();
      ligneCSV = ligneCSV.substring(posComma+1);
    }
  
    /*Serial.println("cocktail.name="+cocktail.name);
    Serial.println("cocktail.author="+cocktail.author);
    for(int i = 0; i<10; i++){
      Serial.println("cocktail.ingredient[i][0]="+cocktail.ingredient[i][0]);
      Serial.println("cocktail.ingredient[i][1]="+cocktail.ingredient[i][1]);
    }*/
  }
  //Serial.println("car="+String(car, DEC));
  Serial.println("readCurrentLineToCocktail - fin");
  
  return car == -1 ? true : false;
}

/*
* Retrieve from SD file the list of cocktails.
*/
Cocktail loadCocktail(byte numOfCocktail)
{
  //Serial.println("loadCocktails - debut");
  File myFile = SD.open("barbabot.txt", FILE_READ);
  
  Cocktail cocktail;
  boolean bEndOfFile = false;
  byte i = 0;
  while(!bEndOfFile && i<numOfCocktail){
    bEndOfFile = readCurrentLineToCocktail(cocktail, myFile);
    i++;
  }
  
  myFile.close();
  
  //Serial.println("loadCocktails - fin");
  return cocktail;
}

/*
* Retrieve from SD file the list of cocktails.
*/
void loadCocktails() 
{
  //Serial.println("loadCocktails - debut");
  
  NUMBER_OF_COCKTAILS = 0;
  File myFile = SD.open("barbabot.txt", FILE_READ);
  Cocktail cocktail;
  
  strcpy(liste_cocktails[NUMBER_OF_COCKTAILS], "JACKPOT");
  
  boolean bEndFile = false;
  bEndFile = readCurrentLineToCocktail(cocktail, myFile);
  while(!bEndFile){
    if(cocktail.name.length() > 0){
      NUMBER_OF_COCKTAILS++;
      char name[31];
      cocktail.name.toCharArray(name, cocktail.name.length()+1);
      strcpy(liste_cocktails[NUMBER_OF_COCKTAILS], name);
    }
    bEndFile = readCurrentLineToCocktail(cocktail, myFile);
  }
  NUMBER_OF_COCKTAILS++;
  for(int i = 0; i<NUMBER_OF_COCKTAILS;i++){
    Serial.println( liste_cocktails[i]);
  }
  
  
  myFile.close();
  
  Serial.println("NUMBER_OF_COCKTAILS ="+ String(NUMBER_OF_COCKTAILS, DEC));
}


