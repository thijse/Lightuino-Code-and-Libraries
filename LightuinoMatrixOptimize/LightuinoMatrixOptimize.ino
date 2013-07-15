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

int noSources = 2; // number of sources used. Maximum: Lightuino_NUMSRCDRVR
int noSinks = 55;  // number of sinks used. Maximum: Lightuino_NUMOUTS

int startMillis;
int endMillis; 


lightuinoAnimatedMatrix mtx(sinks,sources,0,noSources);

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

void startMeasurement()
{
      startMillis = millis();
      //println("startMillis : ");  print(startMillis); 
}

void endMeasurement(int lps)
{
      endMillis = millis();
      //Usb.println("endMillis : "); print(endMillis); 
      double delay = ((float)endMillis-(float)startMillis);
      Usb.print("delay:  "); 
      Usb.println(delay);
}


// Standard Arduino "setup" function
void setup(void)
  {
  // Start up the serial port.  This is not required for the lightuino, I'm just doing it so I can print stuff.
  Serial.begin(9600);
  Serial.println("serial initialized");
  // Start up the Lightuino's USB serial port.

  Usb.begin();
  randomSeed(analogRead(0));
  InitializeMatrix();
  //WordDemoSetup();
 // sinks.setBrightness(255);

  // Say hi so we know its working!
  //println("Word Matrix");
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


  startMillis = millis();
  for (int j=0;j<1000;j++) { mtx.loop();  }  
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
  mtx.animate(0.25);

//  wordMatrix.SetWord(ClockWords[HET]);
//  wordMatrix.SetWord(ClockWords[IS]);
//  wordMatrix.SetWord(ClockWords[VIJF]);


}

void AnimatedMatrixDemo()
{
  println("Animated Matrix demo");
  println("  Animates 20 times between random images");
  
  sinks.setBrightness(255); // Set to full brightness
  
  mtx.clear(0);             // Clear current and next image
  mtx.startAutoLoop();      // Start auto looping  
 
  randomFillMatrix();       // fill new image with random values
  println("   Update image...");  
  mtx.update();             // Show newly created image 

  for (int i=0;i<20;i++) {
  
    println("   Waiting for 3 sec...");
    delay(3000);              // Wait for 5 seconds

    println("   Animating to next image...");  
    randomFillMatrix();       // fill new image with new random values
    mtx.animate(0.25);             // Animate to newly created image 
    
    println("   Waiting for 3 sec...");
    delay(3000);              // Wait for 5 seconds

    println("   Updating to next image...");  
    randomFillMatrix();       // fill new image with new random values
    mtx.update();             // Animate to newly created image 
  }  
  
  mtx.stopAutoLoop();      // Start auto looping
}

void Timing()
{
  int loops = 10000000;
  
  sinks.setBrightness(255); // Set to full brightness
  
  mtx.clear(0);             // Clear current and next image
  mtx.stopAutoLoop();      // No auto looping  
 
  randomFillMatrix();       // fill new image with random values


  //  mtx.update();             // Show newly created image 
  println("Timing animatedTestLoop...");  
  startMeasurement();
  for(int cnt=0;cnt<10000;cnt++) {
    mtx.animatedTestLoop();    
  }
  endMeasurement(loops);  

/*  
    println("Timing animatedTestLoop2...");  
  startMeasurement();
  for(int cnt=0;cnt<10000;cnt++) {
    mtx.animatedTestLoop2();    
  }
  endMeasurement(loops);  
*/
/*
  println("Timing full static loop...");  
  startMeasurement();
  for(int cnt=0;cnt<10000;cnt++) {
    mtx.loop();    
  }
  endMeasurement(loops);  
  
  println("Timing PWM logic in loop..");  
  startMeasurement();
  for(int cnt=0;cnt<10000;cnt++) {
    mtx.pwmLogicLoop();    
  }
  endMeasurement(loops); 

  println("Timing source in loop..");  
  startMeasurement();
  for(int cnt=0;cnt<10000;cnt++) {
    mtx.sourceTestLoop();    
  }
  endMeasurement(loops);   
*/
 


}



void randomFillMatrix() {

  int fillfactor = 50; // fill percentage

  for(int x=0;x<noSinks;x++)      
    for(int y=0;y<noSources;y++)      
        mtx.pixel(x,y, (random(100)<fillfactor)); 
}

void loop(void)
{
   println("main loop"); 
   delay(100);
   //mtx.animatedLoop();
   //AnimatedMatrixDemo();
   Timing();
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




