/*? <section name="examples">
<sketch name="begin_here">
This sketch shows basic Lightuino control, so you can get up and running quickly!
<verbatim>
*/

// Include the appropriate Lightuino library
#include <lightuino5.h>
#include <DigitalPin.h>
#include <Time.h>
#include <MatrixPwm.h>
#include <FlexiTimer2.h>
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

lightuinoAnimatedMatrix mtx(sinks,sources,0,2);  


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
  WordDemoSetup();
 // sinks.setBrightness(255);

#endif                // This line need to be removed if #ifdef is removed too!

  // Say hi so we know its working!
  println("Word Matrix");
}

void InitializeMatrix()
{
    mtx.clear(0);   
    sources.set(0); 
    sinks.set(0,0,0);
}

void MatrixDemo()
{
  int noSources = 2;
  int noSinks = 55;

  int lx = 0;
  int ly = 0;

  for(int x=0;x<55;x++)      
  {
    for(int y=0;y<2;y++)      
    {
      mtx.pixel(lx,ly,0);
      mtx.pixel(x,y,1);
      lx = x;
      ly = y;

      for (int j=0;j<50;j++) { mtx.loop(); delay(1);}  
    }
  }

 // mtx.clear(0);
}

void WordDemo()
{
  //mtx.clear(0);
  //sinks.setBrightness(20);
  
  tmElements_t timeElements;
  time_t clockTime;
  
  int startMillis;
  int endMillis; 
  
  timeElements.Second = 0;
  timeElements.Minute = 55;
  timeElements.Hour   = 19;
  timeElements.Day    = 5; 
  timeElements.Month  = 11;
  timeElements.Year   = 12;
  clockTime = makeTime(timeElements);	 
  
  WordMatrix wordMatrix(mtx);
  wordMatrix.SetTime(clockTime);
  mtx.update();


//  wordMatrix.SetWord(ClockWords[HET]);
//  wordMatrix.SetWord(ClockWords[IS]);
//  wordMatrix.SetWord(ClockWords[VIJF]);
  
  startMillis = millis();
  for (int j=0;j<10000;j++) { mtx.loop();  }  
  endMillis = millis();
  print("Time to loop 10000 times  (ms) : "); 
  println(endMillis-startMillis); 
  
  // 1 loop takes 0.37 ms -> freq = 2690 hz 2.6Mh
  
}

void WordDemoSetup()
{
  //mtx.clear(0);
  //sinks.setBrightness(20);
  
  tmElements_t timeElements;
  time_t clockTime;
  
  timeElements.Second = 0;
  timeElements.Minute = 55;
  timeElements.Hour   = 19;
  timeElements.Day    = 5; 
  timeElements.Month  = 11;
  timeElements.Year   = 12;
  clockTime = makeTime(timeElements);	 
  
  WordMatrix wordMatrix(mtx);
  wordMatrix.SetTime(clockTime);
  mtx.update();
  
  println("manual loop"); 
 for (int j=0;j<10000;j++) { mtx.loop();  }  
  println("manual loop"); 
  for (int j=0;j<10000;j++) { mtx.loop();  }  
  println("Auto loop"); 
  mtx.startAutoLoop();

  for (int j=0;j<30;j++) { 
    delay(100);
    
  }

  timeElements.Second = 0;
  timeElements.Minute = 45;
  timeElements.Hour   = 20;
  clockTime = makeTime(timeElements);	 
  
  wordMatrix.SetTime(clockTime);
  mtx.animate(1);

//  wordMatrix.SetWord(ClockWords[HET]);
//  wordMatrix.SetWord(ClockWords[IS]);
//  wordMatrix.SetWord(ClockWords[VIJF]);


}


void loop(void)
{
  // println("main loop"); 
   delay(100);
   //mtx.animatedLoop();
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



