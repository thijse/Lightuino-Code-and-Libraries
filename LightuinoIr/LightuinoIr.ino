
// Include the appropriate Lightuino library
#include <lightuino5.h>

// These "print" wrappers just output to BOTH USB and UART serial ports (code at the bottom)
void println(char*s);
void print(char*s);
void print(int i,char format=DEC);
void println(int i,char format=DEC);

IrReceiver ir;  // Initialize the IR receiver

// Standard Arduino "setup" function
void setup(void)
{
  // Start up the serial port.  This is not required for the lightuino, I'm just doing it so I can print stuff.
  Serial.begin(9600);
  Usb.begin();
   Serial.println("serial initialized");
  // Say hi so we know its working!
  println("Lightuino 5 IR");
  println("  Waiting for input from your infrared remote -- demo will stop after 5 one-second intervals without input.");
}
  
void Ir(void)
{
    unsigned long int code = ir.read();  // Read a code from the input buffer
    if (code)                            // Nonzero means a code was received.
    {
      // Print it out in hex and binary notation
      /*
      print("  code: ");
      
      unsigned long int codeHigh = code>>32;
      if (codeHigh!= 0) {
        print((unsigned long int)(code>>32),HEX);
        print(" ");
      }
      print((unsigned long int)(code),HEX);
      print(" | ");
      print((unsigned long int)(code>>32),BIN);
      print(" ");
      println((unsigned long int)(code),BIN);
      */
      
      switch (code) {
      case Lightuino_IR_MUTE:
        println("MUTE");    
        break;
      case Lightuino_IR_AVTV:
        println("AV/TV");   
        break;  
     case Lightuino_IR_POWER:
        println("Power");   
        println("Powering down");   
        sleepUntil(Lightuino_IR_POWER);
        break;          
      case Lightuino_IR_LEFT:
        println("left");   
        break;  
      case Lightuino_IR_RIGHT:
        println("right");   
        break;  
      case Lightuino_IR_UP:
        println("up");   
        break;  
      case Lightuino_IR_DOWN:
        println("down");   
        break;   
       case Lightuino_IR_MENU:
        println("Menu");   
        break;  
      case Lightuino_IR_CHPLUS:
        println("CH+");   
        break;
      case Lightuino_IR_CHMINUS:
        println("CH-");   
        break;
      case Lightuino_IR_DASHSLASH:
        println("--/-");   
        break;
      case Lightuino_IR_CIRCLE:
        println("()");   
        break;        
      case Lightuino_IR_VOLMINUS:
        println("Vol-");   
        break;
      case Lightuino_IR_VOLPLUS:
        println("Vol+");   
        break;                
      case Lightuino_IR_0:
        println("0");   
        break;
      case Lightuino_IR_1:
        println("1");   
        break;
      case Lightuino_IR_2:
        println("2");   
        break;
      case Lightuino_IR_3:
        println("3");   
        break;
      case Lightuino_IR_4:
        println("4");   
        break;
      case Lightuino_IR_5:
        println("5");   
        break;
      case Lightuino_IR_6:
        println("6");   
        break;
      case Lightuino_IR_7:
        println("7");   
        break;        
      case Lightuino_IR_8:
        println("8");   
        break;
      case Lightuino_IR_9:
        println("9");   
        break;                
      default:
        print(".");
        break;
      }
      delay(10);
    }
}


void loop(void)
{
   Ir();
}


// These "print" wrappers just output to BOTH serial ports
// but you probably just want to use the Usb serial...
void println(char*s)
{
  Usb.println(s);
}

void print(char*s)
{
  Usb.print(s);
}
void print(int i,char format)
{
  Usb.print(i,format);
}

void println(int i,char format)
{
  Usb.println(i,format);
}
