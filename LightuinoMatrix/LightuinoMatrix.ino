/*? <section name="examples">
<sketch name="begin_here">
This sketch shows basic Lightuino control, so you can get up and running quickly!
<verbatim>
*/

// Include the appropriate Lightuino library
#include <lightuino5.h>
#include <DigitalPin.h>
#include <Time.h>
#include "WordMatrix.h"

#define Lightuino_USB

// These "print" wrappers just output to BOTH USB and UART serial ports (code at the bottom)
void println(char*s);
void print(char*s);
void print(int i,char format=DEC);
void println(int i,char format=DEC);


  
// Create the basic Lightuino 70 LED sink controller (the pins in the 2 40-pin IDE connectors)
LightuinoSink sinks;   
// Create the Lightuino 16 channel source driver controller (the 16 pin connector)
LightuinoSourceDriver sources;

LightuinoMatrix mtx(sinks,sources,0,2);  


//?? Printf-style to the USB
void p(char *fmt, ... )
{
        char tmp[128]; // resulting string limited to 128 chars
        va_list args;
        va_start (args, fmt );
        vsnprintf(tmp, 128, fmt, args);
        va_end (args);
        Usb.print(tmp);
}

//?? A wrapper around delay so you can tweak how long the delays actually are...
/* Do a delay, but also wait for user input if a global var is set */
char waitInput=false;
boolean mydelay(int amt)
{
  char incomingByte =0;
  delay(amt);
  if (waitInput)
    {
      // Wait for input from serial or usb serial
      while ((Serial.available()==0)
#ifdef Lightuino_USB
             && (Usb.available()==0)
#endif
  )
        delay(10);
    }

  if (Serial.available())
    incomingByte = Serial.read();
#ifdef Lightuino_USB
  else if (Usb.available())
    incomingByte = Usb.read();
#endif

  if (incomingByte == 's')
    {
      println("stop");
      waitInput=true;
    }
  else if (incomingByte == 'c')
    {
      println("continue");
      waitInput=false;
    }
  else if (incomingByte == 'n')
    {
      println("next animation");
      return false;
    }
  return true;
}

// Standard Arduino "setup" function
void setup(void)
  {
  // Start up the serial port.  This is not required for the lightuino, I'm just doing it so I can print stuff.
  Serial.begin(9600);
  Serial.println("serial initialized");
  // Start up the Lightuino's USB serial port.
#ifdef Lightuino_USB  // This #ifdef,#endif wrapper means the the code inside will only compile if your Lightuino has a USB port.
                      // That way this sketch will work with multiple versions of the circuitboard.
                      // But since you probably don't care that your sketch does so, you can leave these lines out.

  Usb.begin();
  InitializeMatrix();

#endif                // This line need to be removed if #ifdef is removed too!

  // Say hi so we know its working!
  println("Lightuino 5 Matrix");

  // Create the matrix object.  Pass the source and sink objects, the start scan line, and the total # of lines.  In this case I am doing ALL of them.
  //LightuinoMatrix mtx(sinks,sources,0,2);  
}


/*
void MatrixDemo()
{
      mtx.pixel(0,0,1);
      mtx.pixel(0,1,1);
       mtx.pixel(0,2,1);
       mtx.pixel(0,3,1);      
       mtx.pixel(0,4,1);             
      mtx.pixel(0,5,1);
      mtx.pixel(0,6,1);
      mtx.pixel(0,7,1);      
      mtx.pixel(0,8,1);      
      mtx.pixel(0,9,1);      

  for (int j=0;j<10000;j++) { mtx.loop(); }
  
}
*/

void InitializeMatrix()
{
    mtx.clear(0);   
    sources.set(0); 
    sinks.set(0,0,0);
}

void MatrixDemo()
{
  //println("  Write Pixels");
  int noSources = 2;
  int noSinks = 55;
/*
          mtx.pixel(0,0,1);
          mtx.pixel(4,0,1);
          mtx.pixel(2,1,1);
          mtx.pixel(6,1,1);
          
          mtx.pixel(34,0,1);
         // mtx.pixel(36,0,1);

          for (int j=0;j<1000;j++) { mtx.loop(); delay(3); }
 mtx.clear(0);
*/

  int lx = 0;
  int ly = 0;
  mtx.clear(0);
  sinks.setBrightness(20);
  for(int x=0;x<55;x++)      
  {

    for(int y=0;y<2;y++)      
    {
      /*
      if(random(0,100)<50) {
        mtx.pixel(x,y,0);
      } else {
        mtx.pixel(x,y,1);
      }
      */

      mtx.setSinkSource(lx,ly,0);
      mtx.setSinkSource(x,y,1);
      lx = x;
      ly = y;
      
      /*
      if(random(0,100)<60) {
        mtx.pixel(x,1,0);
      } else {
        mtx.pixel(x,1,1);
      }
  */
      for (int j=0;j<50;j++) { mtx.loop(); delay(1);}
//      for (int j=0;j<5;j++) { mtx.loop(); delay(5);}
  
    }
  }

 // mtx.clear(0);
}


void loop(void)
{
  //Timing();
 MatrixDemo();

}


// These "print" wrappers just output to BOTH serial ports
// but you probably just want to use the Usb serial...
void println(char*s)
{
  Serial.println(s);
#ifdef Lightuino_USB
  Usb.println(s);
#endif
}

void print(char*s)
{
  Serial.print(s);
#ifdef Lightuino_USB
  Usb.print(s);
#endif
}
void print(int i,char format)
{
  Serial.print(i,format);
#ifdef Lightuino_USB
  Usb.print(i,format);
#endif
}

void println(int i,char format)
{
  Serial.println(i,format);
#ifdef Lightuino_USB
  Usb.println(i,format);
#endif
}



