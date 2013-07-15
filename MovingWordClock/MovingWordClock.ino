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
#include "MovingWordMatrix.h"


#define Lightuino_USB

  tmElements_t timeElements;
  time_t clockTime;

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

lightuinoAnimatedMatrix mtx(sinks,sources,0,2);  
MovingWordMatrix wordMatrix(mtx);

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
  Usb.begin();  
  InitializeMatrix();
  WordClockDemoSetup();
  
  //WordDemoSetup();
 // sinks.setBrightness(255);


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

void WordClockDemoSetup()
{
  println("Setting up Word Clock demo");
  println("cycles through times 5 minutes / 5 seconds");
  
  sinks.setBrightness(255); // Set to full brightness
  
  mtx.clear(0);             // Clear current and next image
  mtx.startAutoLoop();      // Start auto looping  
  
  int startMillis;
  int endMillis; 
  
  timeElements.Second = 0;
  timeElements.Minute = 55;
  timeElements.Hour   = 19;
  timeElements.Day    = 5; 
  timeElements.Month  = 11;
  timeElements.Year   = 12;
  clockTime = makeTime(timeElements);	 
  
  print("Setting initial time: ");  
  printDateTime(clockTime);
  
/*
  wordMatrix.SetTime(clockTime);
  
  mtx.update();
  println("   Waiting for 5 sec...");
  delay(5000);              // Wait for 5 seconds
*/



}


void WordClockDemoLoop()
{

    clockTime += 5*60; // add 5 minutes  

    println("Change to next time :");  
    printDateTime(clockTime);
    
    for (int i=0;i<1000;i++) {
      wordMatrix.SetTime(clockTime);
      delay(1);
    }
    delay(3000);              // Wait for 5 seconds 
}

/*
void AnimatedMatrixDemo()
{
  println("Animated Matrix demo");
  println("  Animates 20 times between random images");
  
  sinks.setBrightness(255); // Set to full brightness
  
  mtx.clear(0);             // Clear current and next image
  mtx.startAutoLoop();      // Start auto looping  
 
  randomFillMatrixMany();       // fill new image with random values
  println("   Update image...");  
  mtx.update();             // Show newly created image 
  
  for (int i=0;i<20;i++) {
  
    println("   Waiting for 3 sec...");
    delay(3000);              // Wait for 5 seconds

    println("   Animating to next image...");  
    randomFillMatrixMany();       // fill new image with new random values
    mtx.animate(0.25);             // Animate to newly created image 
    
    //println("   Waiting for 3 sec...");
    delay(3000);              // Wait for 5 seconds

    println("   Updating to next image...");  
    randomFillMatrixMany();       // fill new image with new random values
    mtx.update();             // Animate to newly created image 
  }  
  
  mtx.stopAutoLoop();      // Start auto looping
}


void randomFillMatrixMany() {

  int fillfactor = 50; // fill percentage

  for(int x=0;x<noSinks;x++)      
    for(int y=0;y<noSources;y++)      
        mtx.pixel(x,y, (random(100)<fillfactor)); 
}

void randomFillMatrixSingle()
{
  const int COLUMNS = 11;
  const int ROWS    = 10;

  int x  = random(COLUMNS);
  int y  = random(ROWS);  
	
  // Matrix is 11 columns and 10 rows
  // row is defined right to left
  // row 0,2,4,6,8 use source 0
  // row 1,3,5,7,9 use source 1  
  // colums 0&1, row 0 - use sink 0, column 1&2 use sink 1    
	
  int sinkNo   = (int)((10-x) + ((y/2) * COLUMNS)); // 0..54
  int sourceNo = ( y % 2 );					      // 0..1
		
  mtx.pixel(sinkNo,sourceNo, 1);
}*/

/*
void WordClockDemo()
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
*/


void loop(void)
{
  // println("main loop"); 
   //delay(100);
   //mtx.animatedLoop();
//   WordMatrixDemo();
//   AnimatedMatrixDemo();

  WordClockDemoLoop();
  //setSingle();
//  setSingle2();
}

void setSingle() {

/*
 for(int y=0;y<10;y++) {      
   for(int x=0;x<11;x++) {     

       mtx.clear(0);
       wordMatrix.SetPixel(x,y,1);
       mtx.update();
       delay(1000);
    }
  }
  */
      int xx;
      for(int y=0;y<noSources;y++) {             
       for(int x=0;x<noSinks;x++) {      
         
         // fix for wrongly connected leds
         if      (x == 33) { xx = 55; }
         else if (x == 34) { xx = 56; }        
	 else                xx = x;

         print("Sink : "); 
         print(x); 
         print(" = "); 
         print(xx);         
         print("Source : "); 
         println(y); 


         mtx.pixel(xx,y, 1); 
         mtx.update();
         delay(200);
      }
    }

}

void setSingle2() {


 for(int y=5;y<10;y++) {      
   for(int x=0;x<11;x++) {     

       mtx.clear(0);
       wordMatrix.SetPixel(x,y,1);
       //mtx.update();
       mtx.animate(0.25);
       delay(2000);
    }
  }
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


// convenience function to display date and time
void printDateTime(time_t time) {
    TimeElements _timeElements;
    breakTime(time, _timeElements); 
    Usb.print(_timeElements.Hour);
    Usb.print(":");
    Usb.print(_timeElements.Minute);
    Usb.print(":");
    Usb.print(_timeElements.Second);
    Usb.print(" ");
    Usb.print(_timeElements.Day, DEC);
    Usb.print("-");
    Usb.print(_timeElements.Month, DEC);
    Usb.print("-");
    Usb.println(_timeElements.Year+1970, DEC);
}

