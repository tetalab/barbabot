//Video2Minitel
//by Renaud, Fabrice & Phil
//Inspired from Video2ledwallHarpSerial V1.0 by Fabrice Fourc
// http://www.tetalab.org


import processing.video.*;
import processing.net.*;
int plage=44;//256;
int MINITEL_CHAR_WIDTH = 50;//*2;
int MINITEL_CHAR_HEIGHT = 50;//*4;
	// en fait ce n'est pas vraiment un pixel par pixel char...
int PIXEL_CHAR_WIDTH = 1;
int PIXEL_CHAR_HEIGHT = 1;
	
// Size of each cell in the grid, ratio of window size to video size
int videoScale = 10;
int videoScalex = 1;
int videoScaley = 1;
int leftdep = 0;


// Number of columns and rows in our system
int cols, rows;
// Variable to hold onto Capture object
public int s = 50;
import processing.serial.*;

// The serial port:
Serial myPort;

//luminosite globale de la colone pour le son
int colValue;
String ledCol;
String ledWallMsg;

//Client myClient;
Capture video;


boolean PenDown;
int timerUpDown = 300;

void liftPen() 
{
  if (PenDown)
  {   
    myPort.write("SP,0\r");           
    PenDown = false;
    delay(timerUpDown);
  }
}

void lowerPen() 
{
  if (!PenDown)
  {     
  myPort.write("SP,1\r");           
  PenDown = true;
  delay(timerUpDown);
  }
}


 void Up() 
{
//   myPort.write("SM,100,10,0\r");           
   myPort.write("SM,15,15,0\r");           
  //"SM,<duration>,<penmotor steps>,<eggmotor steps><CR>"
} 

 void Down() 
{
//   myPort.write("SM,100,-10,0\r");           
   myPort.write("SM,15,-15,0\r");           
  //"SM,<duration>,<penmotor steps>,<eggmotor steps><CR>"
}   
  
  
 void Left() 
{
//   myPort.write("SM,100,0,10\r");           
   myPort.write("SM,15,0,15\r");           
  //"SM,<duration>,<penmotor steps>,<eggmotor steps><CR>"
} 

 void Right() 
{
//   myPort.write("SM,100,0,-10\r");           
   myPort.write("SM,15,0,-15\r");           
  //"SM,<duration>,<penmotor steps>,<eggmotor steps><CR>"
}   



void setup() 
{
  size(MINITEL_CHAR_WIDTH*PIXEL_CHAR_WIDTH*videoScalex,MINITEL_CHAR_HEIGHT*PIXEL_CHAR_HEIGHT*videoScaley);
  frameRate(25);
  
  // List all the available serial ports:
  println(Serial.list());

  // I know that the first port in the serial list on my mac
  // is always my  Keyspan adaptor, so I open Serial.list()[0].
  // Open whatever port is the one you're using.
  myPort = new Serial(this, Serial.list()[1], 115200);

  //myClient = new Client(this, "127.0.0.1", 5204);
  // Initialize columns and rows
  cols = width/videoScalex;
  rows = height/videoScaley;
  video = new Capture(this,cols,rows,30);
}


//----------------------------------------------------------


boolean getPixel(int x,int y)
{
  println("x" + x + "y" + y*video.width);
  color c = video.pixels[x + y*video.width];
  int value = (int)brightness(c);  // get the brightness
  if (value<s) return false;
  else return true;
}lllllll


void draw() {
  // Read image from the camera
  noStroke();

  if (video.available()) 
  {
    video.read();
  }
  video.loadPixels();
  
  if(!mousePressed) //si je ne clique pas, j'affiche le preview
  {
    image(video,0,0);
    // Begin loop for columns
    for (int i = 0; i < cols; i++) 
    {
      // Begin loop for rows
      for (int j = 0; j < rows; j++) 
      {
        // Where are we, pixel-wise?
        int x = i*videoScalex;
        int y = j*videoScaley;
        // Looking up the appropriate color in the pixel array
        color c = video.pixels[i + j*video.width];
        int value = (int)brightness(c);  // get the brightness
        if (value<s)
        {
          fill(0);
        }
        else
        {
          fill(255);
        }
        rect(x,y,videoScalex,videoScaley);
      }
      ledWallMsg += ledCol;
    }
  }
  else //si je clique sur l'image j'envoi sur le port serie
  {
    
    for (int y=0;y<MINITEL_CHAR_HEIGHT;y++) 
    {
      for (int x=0;x<MINITEL_CHAR_WIDTH;x++) 
      {
        if(getPixel(x,y)) lowerPen();
        else liftPen();
        Left();
        leftdep++;
      }
      leftdep--;
      lowerPen();      
      while(leftdep>0)
      {
        Right();
        leftdep--;
        println("retour");
      }
      Down();
    }
  }
}

void keyPressed() //reglage du seuil (a-, z+)
{
 if( key == 'a') 
 {
   s = s - 1;
   println("s " + s);
 }
 if ( key == 'z')
 {
   s = s + 1;
   println("s " + s);
 }
 
 if( key == ' ') 
  {

    if (PenDown)
      liftPen();
    else
      lowerPen();
  }
  
    if( key == 'u') 
  {
    Up();
  }
  
    if( key == 'd') 
  {
    Down();
  }
 
   
    if( key == 'l') 
  {
    Left();
  }
  
    if( key == 'r') 
  {
    Right();
  } 
  
 
 
 
 
}

