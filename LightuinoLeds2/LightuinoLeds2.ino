/*? <section name="examples">
<sketch name="begin_here">
This sketch shows basic Lightuino control, so you can get up and running quickly!
<verbatim>
*/

// Include the appropriate Lightuino library
#include <lightuino5.h>
#define Lightuino_USB
#include <DigitalPin.h>

// These "print" wrappers just output to BOTH USB and UART serial ports (code at the bottom)
void println(char*s);
void print(char*s);
void print(int i,char format=DEC);
void println(int i,char format=DEC);

// Create the basic Lightuino 70 LED sink controller (the pins in the 2 40-pin IDE connectors)
LightuinoSink sinks;   
// Create the Lightuino 16 channel source driver controller (the 16 pin connector)
LightuinoSourceDriver sources;

// This object PWMs the Lightuino sinks allowing individual LED brightness control, and provides array-based access to the Leds
FlickerBrightness pwm(sinks);

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
  sinks.setBrightness(20);

#endif                // This line need to be removed if #ifdef is removed too!

  // Say hi so we know its working!
  println("Lightuino 4/5 Introduction Sketch V5.0");
  }
  



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


void mydelay_old(int amt)
{
  delay(amt);
}

//?? This function demonstrates turning the 70 sinks on/off.  The Lightuino can turn LEDs ON or OFF and not use any additional CPU. 
//   To implement dimming requires the PWM (Pulse Width Modulation -- google it) technique, which is shown in the SinkPwmDemo
//   function below.  Since both techniques are using the same LEDs, they can't be used at the same time.  Instead use the PWM
//   technique only and set the LED intensity to off (0) or full (Lightuino_MAX_BRIGHTNESS-1).
void SinkOnOffDemo()
  {
  println("Sink Discrete (On/OFF) Control Demo");
  sources.set(0xffff); 
  
  println("  Turn them all on!");  
  sinks.set(0xffffffff,0xffffffff,B00111111);  // Each bit in these 3 numbers corresponds to one LED light
  mydelay(4000);

  println("  Turn on sequentially");  
  sinks.set(0,0,0);
  unsigned long int a=0,b=0;
  unsigned char c=0;
  for (int i=0;i<70;i++)   // Instead of 70 you can use the constant "Lightuino_NUM_SINKS"
    {
      p("index %d\n",i);
      setbit(i,&a,&b,&c);  // Set a particular bit in the a,b,c variables
      sinks.set(a,b,c);    // Set the LEDs to be on/off based on these bits.
      mydelay(100);        // Wait
      clearbit(i,&a,&b,&c); // Set a particular bit in the a,b,c variables to 0
      mydelay(1);
    }
  if (0) for (int i=0;i<Lightuino_NUM_SINKS;i++)
    {
      clearbit(i,&a,&b,&c); // Set a particular bit in the a,b,c variables to 0
      sinks.set(a,b,c);
      mydelay(100);
    }
 
  if (0) for (int i=0;i<5;i++)
    {
    println("  Turn on every other LED");
    
    // Each bit (1 or 0) in this array corresponds to one LED light
    byte ledState[9] = {B10101010,B10101010,B10101010,B10101010,B10101010,B10101010,B10101010,B10101010,B10101010};
     
    // Now send it to the chips.
    sinks.set(ledState);  
      
    mydelay(250);
  
    println("  Now put in the opposite pattern");
    // Now set up another pattern
    for (int j=0;j<9;j++) ledState[j] = B01010101;
    
    // Now send it to the chips.
    sinks.set(ledState);  
      
    mydelay(250);
    }
   
  println("  Turn them all off!");  
  sinks.set(0,0,0);
  
  // Its THAT simple!
  
  // But before you go off and reinvent the wheel on top of the functions shown here, please check 
  // out the more advanced animation functions used in the other sketches (for example lightuino_animations).  

  // Mastering them will will let you code complex patterns with less effort than doing it yourself.
}

//?? Turn all the LEDs and source drivers off
void AllOff(void)
{
  sources.set(0); 
  sinks.set(0,0,0);
}

//?? Turn all the LEDs and source drivers off
void AllOn(void)
{
  //sources.set(0xffff); 
  sources.set(3); 
//  sinks.set(0xffffffff,0xffffffff,0xff);
  sinks.set(0xffffffff,0x00000fff,0);
}


void AlternateOn(void)
{
  byte ledState[9] = {B10101010,B10101010,B10101010,B10101010,B10101010,B10101010,B10101010,B10101010,B10101010};
  sinks.set(ledState);
  
  for (int i=0;i<50;i++)
  {
    println("  sources off");
    sources.set(0);
    delay(2000);
    println("  sources on");
    sources.set(1); 
    delay(2000);
  }
  
  sources.set(0xffff); 
}

void WalkThroughLeds(void)
{
  unsigned long int a1;
  unsigned long int a2;
  unsigned long int a3;
  unsigned char leds;
  sources.set(1); 
  
  leds = 1;
  
  for (int i=0;i<55;i++)
  { 
    const int step1 = 0;
    const int step2 = 32;
    const int step3 = 32*2;
    
    if (i==step3) {
      a2 = 0;
      a3 = 1;
    }  
    else if (i>step3) {
      a3 = a3 << 1;
    }
    else if (i==step2) {
      a1 = 0;
      a2 = 1;
    }  
    else if (i>step2) {
      a2 = a2 << 1;
    }
    else if (i==step1) {
      a1 = 1;
      a2 = 0;      
      a3 = 0;            
    }  
    else if (i>step1) {
      a1 = a1 << 1;
    }
    
    
    println("shift");
    sinks.set(a1,a2,a3);
    delay(200);
  }
  

}



void SingleLed()
{
    println("  Turn off the entire matrix");
    LightuinoMatrix mtx(sinks,sources,0,2);  
    mtx.clear(0);    
    mtx.pixel(1,1,1);
}


void loop(void)
{
  
  //SingleLed();
  
//  AlternateOn();

//  println("MatixDemo");
  
  
//  MatrixDemo();
  
  println("Turn all leds on");
  //AllOn();
  delay(400);
 println("Turn every other led on");
 //AlternateOn();
 WalkThroughLeds();
 // delay(4000);
  println("Turn all leds off");  
  AllOff();  // When the board boots up there will be random values in various chips resulting in some lights being on.  So its good to turn everything off right away.
  delay(4000);

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


/*? </verbatim></sketch></section>
*/

