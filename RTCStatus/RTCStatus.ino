#include <lightuino5.h>
#include <Wire.h>
#include <Time.h>
#include <Utils.h>
#include <DS1307RTC.h>
#include <MemIO.h>

int addressRamTime;
int addressStoreCounter;



// Set up of interactive shell
void setup() {
    //Serial.begin(9600);
      Usb.begin();
    // Set shell commands
    Usb.println("Starting...");
    
    // Always get the adresses first and in the same order
   // addressRamTime      = RTC.RAM->getAddress(sizeof(time_t));
//    addressStoreCounter = RTC.RAM->getAddress(sizeof(uint16_t));
    Status();
   
}

// Process interactive shell
void loop() {
  // Process shell input
  delay(1000);
  getTime();
  
}

// Time
void getTime()
{
    printDateTime(RTC.get());
}


// Define "STATUS" command.
void Status()
{
  RTC.get(); // try and fetch time. This will search for chip
  if (RTC.chipPresent()) { 
       Serial.println("RTC clock was found"); 
  } else { 
       Serial.println("RTC clock was not found"); 
  }            
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
