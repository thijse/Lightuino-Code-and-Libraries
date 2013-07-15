#include <lightuino5.h>
#include <Utils.h>
#include <Wire.h>
#include <Time.h>
#include <TimeUtils.h>
#include <MemIO.h>
#include <DS1307RTC.h>
#include <DCF77.h>
#include <RTCDCF77.h>

#define DCF_PIN 2	         // Connection pin to DCF 77 device
#define DCF_INTERRUPT 0		 // Interrupt number associated with pin

DCF77 DCF = DCF77(DCF_PIN,DCF_INTERRUPT,false);

time_t startTime;
TimeUtils* convertTime = new TimeUtils();  
RTCDCF77 *RTCDCF;

// Set up of interactive shell
void setup() {
    Usb.begin();
    delay(1000);
    Usb.println("Starting...");
    RTCDCF = new RTCDCF77(&RTC, &DCF); // create an instance for the user
    
    delay(500);
    // set initial time
    DCF.Start();
}

// Process interactive shell
void loop() {
  Usb.print("Loop"); 
  // Process shell input
  delay(100);
  update();
  getTime();
}


// Define "TIME" command.
void getTime()
{     
    DCF.Stop();
    delay(100);
    // Read the current time.
    Usb.print("Curr RTCDCF: ");  
    printDateTime(RTCDCF->getUncorrected());
    Usb.print("Corr RTCDCF: ");  
    printDateTime(RTCDCF->get());
     DCF.Start();       
    delay(100);
}

// Define "Set" command.
void update()
{    
    Usb.println("Setting DCF time");      
    DCF.Stop();
    delay(100);
    if (RTCDCF->update())
    {
      Usb.println("Succesful update of DCF time");
    } else {
      Usb.println("Unsuccesful update of DCF time");
    }
    DCF.Start();       
    delay(100);
}


// Define "STATUS" command.
void status()
{
  DCF.Stop();
  delay(100);
  
//  tmElements_t timeElements; 
//  timeElements.Day = 180;
  
 // time_t maxTime = makeTime(timeElements)
 // Usb.println("Getting Status");
  
  
  Stat status = RTCDCF->getStatus();
  Usb.println("Getting Status");

  Usb.print("Previous ref time ");
  printDateTime(status.previousRefTime);
  
  Usb.print("Summed time drift (s)");
  Usb.println(status.drift);
	  
  Usb.print("Summed time interval (s)");
  Usb.println(status.interval);

  Usb.print("Number of time updates ");  
  Usb.println(status.updates);
  
  // Calculate corrections to be used
  double correctionFactor = (double)status.drift/(double)status.interval;
  double secondOrder      = 1.0 / (1.0-sq(correctionFactor));

  Usb.print("Drift correction factor ");
  Usb.println(correctionFactor,5);
  
  Usb.print("DCFRTC uncorrected time ");
  printDateTime(RTCDCF->getUncorrected());
  
  Usb.print("DCFRTC corrected time ");  
  printDateTime(RTCDCF->get());
  Usb.print("DCF  time ");  
  DCF.fetchTimeUpdate();
  printDateTime(DCF.getTime());
  DCF.Start();       
  delay(100);  
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
