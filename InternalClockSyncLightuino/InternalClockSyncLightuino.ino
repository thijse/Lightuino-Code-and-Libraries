/*
 * InternalClockSync.pde
 * example code illustrating time synced from a DCF77 receiver
 * Thijs Elenbaas, 2012
 * This example code is in the public domain.
 
  This example shows how to fetch a DCF77 time and synchronize
  the internal clock. In order for this example to give clear output,
  make sure that you disable logging  from the DCF library. You can 
  do this by commenting out   #define VERBOSE_DEBUG 1   in Utils.cpp. 
 */
#include <lightuino5.h>
#include <DCF77.h>
#include <Time.h>
#include <Utils.h>

#define DCF_PIN 2	         // Connection pin to DCF 77 device
#define DCF_INTERRUPT 0		 // Interrupt number associated with pin

time_t time;
DCF77 DCF = DCF77(DCF_PIN,DCF_INTERRUPT,false);


void setup() {
  //Serial.begin(9600); 
  Usb.begin();
  setTime(0);
  DCF.Start();  
  println("Waiting for DCF77 time ... ");
  println("It will take at least 2 minutes until a first update can be processed.");
}

void loop() {
  delay(1000);
  if(DCF.fetchTimeUpdate())  {
    time_t DCFtime = DCF.getTime(); // Check if new DCF77 time is available   
    println("Time is updated");
    setTime(DCFtime);
  }	
  digitalClockDisplay();  
}

void digitalClockDisplay(){
  // digital clock display of the time
  print(hour());
  printDigits(minute());
  printDigits(second());
  print(" ");
  print(day());
  print(" ");
  print(month());
  print(" ");
  print(year()); 
  println(); 
}

void printDigits(int digits){
  // utility function for digital clock display: prints preceding colon and leading 0
  print(":");
  print(digits/10);
  print(digits%10);
//  print(digits);
}
