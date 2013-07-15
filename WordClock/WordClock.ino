/*? <section name="examples">
<sketch name="begin_here">
This sketch shows basic Lightuino control, so you can get up and running quickly!
<verbatim>
*/

// Include the appropriate Lightuino library
#include <lightuino5.h>
#define Lightuino_USB
// If you have a shield, include L4 instead
//#include <lightuino4.h>

// These "print" wrappers just output to BOTH USB and UART serial ports (code at the bottom)
void println(char*s);
void print(char*s);
void print(int i,char format=DEC);
void println(int i,char format=DEC);

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

#endif                // This line need to be removed if #ifdef is removed too!

  // Say hi so we know its working!
  println("Lightuino 4/5 Introduction Sketch V5.0");
  }
  
// Create the basic Lightuino 70 LED sink controller (the pins in the 2 40-pin IDE connectors)
LightuinoSink sinks;   
// Create the Lightuino 16 channel source driver controller (the 16 pin connector)
LightuinoSourceDriver sources;


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
  
  println("  Turn them all on!");  
  sinks.set(0xffffffff,0xffffffff,B00111111);  // Each bit in these 3 numbers corresponds to one LED light
  mydelay(4000);

  println("  Turn on sequentially");  
  sinks.set(0,0,0);
  unsigned long int a=0,b=0;
  unsigned char c=0;
  sources.set(1);
  for (int j=0;j<2;j++)   // Go through all used sources
  {        
    for (int i=0;i<11;i++)   //Go through all used sinks
      {
        p("index %d\n",i);
        setbit(i,&a,&b,&c);  // Set a particular bit in the a,b,c variables
        sinks.set(a,b,c);    // Set the LEDs to be on/off based on these bits.
        mydelay(250);        // Wait
        clearbit(i,&a,&b,&c); // Set a particular bit in the a,b,c variables to 0
        sinks.set(a,b,c);
        mydelay(1);
      }
      sources.shift(1); // to to next row
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
  sources.set(0xffff); 
  sinks.set(0xffffffff,0xffffffff,0xff);
}


void OneOn(void)
{
  byte ledState[9] = {B10101010,B10101010,B10101010,B10101010,B10101010,B10101010,B10101010,B10101010,B10101010};
  sinks.set(ledState);
  sources.set(0xffff); 
}

  

//?? This function demonstrates control over the source drivers (the 16 pin header on top)
void SourceDriverDemo()
{
  // clk,data,strobe,enable
  println("Source Driver Demo");

#if 0
  println("Source 1");
  sources.set(B1);
  mydelay(500);
  println("Source 2");
  sources.set(B10);
  mydelay(500);
  println("Source 3");
  sources.set(B100);
  mydelay(500);
#endif
  
  println("  Driving alternating patterns");
  if(1) for (int i=0;i<10;i++)
    {
      println("  5");
      sources.set(0x5555);
      mydelay(250);
      
      
      println("  a");
      sources.set(0xaaaa);
      mydelay(250);
    }

  println("  Shifting 1 set bit (per 16 bits) through the chips.");
  sources.set(0x0000);
  for (int i=0;i<25;i++)
    {
      mydelay(250);
      sources.shift(((i&15)==0));
    }
    
  sources.set(0);  // All done, so turn them all off
}



void MatrixDemo(LightuinoSink& sink)
{
  println("LED Matrix Demo");
  
  // Create the matrix object.  Pass the source and sink objects, the start scan line, and the total # of lines.  In this case I am doing ALL of them.
  LightuinoMatrix mtx(sink,sources,0,16);  

  println("  Turn on the entire matrix");
  mtx.clear(1);
  // You've got to keep calling loop to paint each scan line in the matrix
  for (int j=0;j<5000;j++) { mtx.loop();}
 
  println("  Write a bit pattern into the entire matrix");
  memset(mtx.videoRam,0xAA,((Lightuino_NUMOUTS/8)+1)*Lightuino_NUMSRCDRVR);
  // You've got to keep calling loop to paint each scan line in the matrix
  for (int j=0;j<5000;j++) { mtx.loop();}

  println("  Turn off the entire matrix");
  mtx.clear(0);
 
  println("  Write Pixels");
  for(int x=0;x<Lightuino_NUMOUTS;x++)
    {
      for(int y=0;y<Lightuino_NUMSRCDRVR;y++)
        {
          mtx.pixel(x,y,1);
          for (int j=0;j<10;j++) { mtx.loop();}
        }
    }

  mtx.clear(0);

}


void loop(void)
{

//  println("Turn all leds on");
//  AllOn();
  delay(100);
 println("Turn one led on");
  AllOn();
  delay(4000);
  println("Turn all leds off");  
  AllOff();  // When the board boots up there will be random values in various chips resulting in some lights being on.  So its good to turn everything off right away.
//  delay(4000);

#if 0
  SourceDriverDemo();
  SinkOnOffDemo();
  SinkPwmDemo();
  
  //MatrixDemo(sinks);
   
   LightSensorDemo();
   IrDemo();
#endif   
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

