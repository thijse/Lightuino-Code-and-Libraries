/*
  EEPROMEx.cpp - Extended EEPROM library
  Copyright (c) 2012 Thijs Elenbaas.  All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

/******************************************************************************
 * Includes
 ******************************************************************************/
#include "lightuino.h"
#include "MovingWordMatrix.h"
#include "lightuinoAnimatedMatrix.h"
#include <Time.h>        //http://www.arduino.cc/playground/Code/Timevs

/******************************************************************************
 * Definitions
 ******************************************************************************/

 #define _WORDMATRIX_VERSION 1 // software version of this library
 
/******************************************************************************
 * Constructors
 ******************************************************************************/

// By default we choose conservative settings
MovingWordMatrix::MovingWordMatrix(lightuinoAnimatedMatrix& _matrix) : matrix(_matrix)
{	
}

/******************************************************************************
 * User API
 ******************************************************************************/

 // Todo: move to utils
 static inline int8_t sgn(int val) {
  if (val < 0) return -1;
  if (val==0) return 0;
  return 1;
}
 
static inline int max(int val1, int val2) {
  return (val1 >  val2) ? val1 : val2;
}

static inline int abs(int val) {
  return (val*sgn(val));
} 
 
void MovingWordMatrix::SetPixel(unsigned char x,unsigned char y,unsigned char val)
{
    // map physical matrix on sinks and sources
		
	const int COLUMNS = 11;
	const int ROWS    = 10;
	
	// Matrix is 11 columns and 10 rows
	// row is defined right to left
	// row 0,2,4,6,8 use source 0
	// row 1,3,5,7,9 use source 1
	// colums 0&1, row 0 - use sink 0, column 1&2 use sink 1    
	
	int sinkNo   = (int)((10-x) + ((y/2) * COLUMNS)); // 0..54
	int sourceNo = ( y % 2 );					      // 0..1
		
	matrix.pixel(sinkNo,sourceNo, val);
}

void MovingWordMatrix::SetWord(const unsigned char ClockWord[])
{
	for (int i = 0; i < ClockWord[2]; i++)
    {
		SetPixel(ClockWord[0]+i,ClockWord[1],1);
	}
}

void MovingWordMatrix::moveWord(const unsigned char fromWord[], const unsigned char toWord[])
{

	unsigned char moveWord[3];
	
   // Determine move and grow direction
    int directionX    = sgn(fromWord[0]-toWord[0]);
    int directionY    = sgn(fromWord[1]-toWord[1]);
	int directionGrow = sgn(fromWord[2]-toWord[2]);
	
	// Determine move steps
	int moveStepsX    = abs(fromWord[0]-toWord[0]);	
	int moveStepsY    = abs(fromWord[1]-toWord[1]);
	int growSteps     = abs(fromWord[2]-toWord[2]);
	int totalSteps     = max(max(moveStepsX,moveStepsY),growSteps);   
   
    Usb.print("direction [");
	Usb.print(directionX);
	Usb.print(", ");
	Usb.print(directionY);
	Usb.print(", ");
	Usb.print(directionGrow);
	Usb.print("] , stepsize [");
	Usb.print(moveStepsX);
	Usb.print(", ");
	Usb.print(moveStepsY);
	Usb.print(", ");
	Usb.print(growSteps);
	Usb.println("]");  
   
	if (animationStep< moveStepsX) {  
		moveWord[0] = fromWord[0] + animationStep*directionX;
	} else {
		moveWord[0] = toWord[0]; 
	}
   
	if (animationStep< moveStepsY) {  
		moveWord[1] = fromWord[1] + animationStep*directionY;
	} else {
		moveWord[1] = toWord[1]; 
	}
      
   if (animationStep< growSteps) {  
		moveWord[2] = fromWord[2] + animationStep*directionGrow;
	} else {
		moveWord[2] = toWord[2]; 
	}
  
  
	Usb.print("step ");  
	Usb.print(animationStep);  
    Usb.print(" : From [");
	Usb.print(fromWord[0]);
	Usb.print(", ");
	Usb.print(fromWord[1]);
	Usb.print(", ");
	Usb.print(fromWord[2]);
	Usb.print("] to [");
	Usb.print(toWord[0]);
	Usb.print(", ");
	Usb.print(toWord[1]);
	Usb.print(", ");
	Usb.print(toWord[2]);
	Usb.print("], current [");
	Usb.print(moveWord[0]);
	Usb.print(", ");
	Usb.print(moveWord[1]);
	Usb.print(", ");
	Usb.print(moveWord[2]);
	Usb.println("]");
	
	// Set moving word
	SetWord(moveWord);	
   
	// Add to global finishedAnimation. finishedAnimation is only be true, if all movements finished	
	//finishedAnimation = finishedAnimation && (animationStep >= totalSteps);
	
	bool finishedOne = (animationStep >= totalSteps);
	Usb.print("finished this word: ");
	Usb.print(finishedOne);
	finishedAnimation = finishedAnimation && finishedOne;
	Usb.print(", all words: ");
	Usb.println(finishedAnimation);
	
}

bool MovingWordMatrix::updateMove() {
	if(!finishedAnimation) {		    
		// Ongoing animation, check if matrix is ready for next transition
		if (!matrix.isAnimating()) {
			Usb.println("Ready for next animation");
			animationStep++;
			MoveTime(currHour,currMin);	
		} else {
			Usb.println("Previous animation still running");
		}
		return true;
	} else {
			Usb.println("Finished moving");
	}
}

void MovingWordMatrix::SetTime(time_t time)
{
	tmElements_t timeinfo;	
	breakTime(time, timeinfo);		
	
	if(finishedAnimation) {		    
		// Moving done, check if we should update the time
		if(timeinfo.Minute==currMin && timeinfo.Hour == currHour) {
		    // time did not change 
			Usb.println("time did not change");
		} else {
		    // time changed, start new animation 
			Usb.println("time changed, start new moving ");
			currMin = timeinfo.Minute;
			currHour = timeinfo.Hour;		
			finishedAnimation = true;  // the moving words will set this to false if not ready
			animationStep = 0;
			MoveTime(currHour,currMin);
			matrix.animate(0.25); 
		}
	} else {
		// Ongoing animation, check if matrix is ready for next transition
		if (!matrix.isAnimating()) {
			Usb.println("Ready for next animation");
			animationStep++;
			MoveTime(currHour,currMin);	
			matrix.animate(0.25); 
		} else {
			Usb.println("Previous animation still running");
		}
        // if not, exit and wait for next time		
	}
}

void MovingWordMatrix::MoveTime(int currHour, int currMinute)
{		
	int min     = currMinute % 5;
	int minDiv5 = currMinute / 5;
	int hour    = currHour > 12 ? currHour -12 : currHour;
	
	// It is ..
	ItIs();
	
	// 5 minute intervals
	switch ( minDiv5 ) {
		case 0 : 
			Min0(); break;
		case 1 : 
			Min5(); break;
		case 2 : 
			Min10(); break;
		case 3 : 
			Min15(); break;
		case 4 : 
			Min20(); break;
		case 5 : 
			Min25(); break;
		case 6 : 
			Min30(); break;
		case 7 : 
			Min35(); break;
		case 8 : 
			Min40(); break;
		case 9 : 
			Min45(); break;
		case 10 : 
			Min50(); break;
		case 11 : 
			Min55(); break;
	}

	// Plus minutes indicators
	switch ( min ) {
		case 0 : 
			break;
		case 1 : 
			Plus1(); break;
		case 2 : 
			Plus2(); break;
		case 3 : 
			Plus3(); break;
		case 4 : 
			Plus4(); break;
	}

	// hours
	switch ( hour ) {
		case 1 : 
			Hour1(); break;
		case 2 : 
			Hour2(); break;
		case 3 : 
			Hour3(); break;
		case 4 : 
			Hour4(); break;
		case 5 :  
			Hour5(); break;
		case 6 : 
			Hour6(); break;
		case 7 : 
			Hour7(); break;
		case 8 : 
			Hour8(); break;
		case 9 :  
			Hour9(); break;
		case 10 : 
			Hour10(); break;
		case 11 : 
			Hour11(); break;
		case 12 : 
			Hour12(); break;
	}

}

void inline MovingWordMatrix::ItIs() 
{
	Usb.println("set HET IS");
	SetWord(ClockWords[HET]);
	SetWord(ClockWords[IS]);	
}

void inline MovingWordMatrix::Min0()
{	
    // Animate VIJF VOOR ... to ... UUR
	Usb.println("Animate VIJF VOOR ... to ... UUR");
    moveWord(ClockWords[VIJF],ClockWords[UUR]);
	moveWord(ClockWords[VOOR],ClockWords[UUR]);		
}

void inline MovingWordMatrix::Min5()
{
	// Animate ...  UUR to VIJF OVER ... 
	Usb.println("Animate ...  UUR to VIJF OVER ... ");
    moveWord(ClockWords[UUR],ClockWords[VIJF]);
	moveWord(ClockWords[UUR],ClockWords[OVER]);		
	Usb.println("VIJF OVER");
}

void inline MovingWordMatrix::Min10()
{
   // Animate VIJF OVER ... to TIEN OVER ... 
    Usb.println("Animate VIJF OVER ... to TIEN OVER ...");
    moveWord(ClockWords[TIEN],ClockWords[VIJF]);
	SetWord(ClockWords[OVER]);
	Usb.println("TIEN OVER");
}

void inline MovingWordMatrix::Min15()
{
    // Animate TIEN OVER ...  to KWART OVER ... 
    Usb.println("Animate TIEN OVER ...  to KWART OVER ... ");	
	moveWord(ClockWords[TIEN],ClockWords[KWART]);
	moveWord(ClockWords[OVER],ClockWords[OVER2]);	
	Usb.println("KWART OVER");
}

void inline MovingWordMatrix::Min20()
{
	// Animate KWART OVER ...  to TIEN VOOR HALF ... 	
    Usb.println("Animate KWART OVER ...  to TIEN VOOR HALF ... 	");
	moveWord(ClockWords[KWART],ClockWords[TIEN]);
	moveWord(ClockWords[OVER2],ClockWords[VOOR]);	
	moveWord(ClockWords[OVER2],ClockWords[HALF]);	
	Usb.println("TIEN VOOR HALF");
}

void inline MovingWordMatrix::Min25()
{
	// Animate TIEN VOOR HALF ... to VIJF VOOR HALF ...
    Usb.println("Animate TIEN VOOR HALF ... to VIJF VOOR HALF ...");
	moveWord(ClockWords[TIEN],ClockWords[VIJF]);
	SetWord(ClockWords[VOOR]);
	SetWord(ClockWords[HALF]);
	Usb.println("VIJF VOOR HALF");
}

void MovingWordMatrix::Min30()
{
   // Animate VIJF VOOR HALF ...to HALF ...
   Usb.println("Animate VIJF VOOR HALF ...to HALF ...");
   moveWord(ClockWords[VIJF],ClockWords[HALF]);
   moveWord(ClockWords[VOOR],ClockWords[HALF]);
   SetWord(ClockWords[HALF]);
	Usb.println("HALF");
}

void inline MovingWordMatrix::Min35()
{
	// Animate HALF .. to VIJF OVER HALF ...
	Usb.println("HALF .. to VIJF OVER HALF ...");
	moveWord(ClockWords[HALF],ClockWords[VIJF]);
	moveWord(ClockWords[HALF],ClockWords[OVER]);
	SetWord(ClockWords[HALF]);
	Usb.println("VIJF OVER HALF");
}

void inline MovingWordMatrix::Min40()
{
	// Animate VIJF OVER HALF  .. to TIEN OVER HALF ...
	Usb.println("Animate VIJF OVER HALF  .. to TIEN OVER HALF ..");
	moveWord(ClockWords[VIJF],ClockWords[TIEN]);
	SetWord(ClockWords[OVER]);
	SetWord(ClockWords[HALF]);
	Usb.println("TIEN OVER HALF");
}

void inline MovingWordMatrix::Min45()
{
	// Animate VIJF OVER HALF ... to KWART VOOR ...
	Usb.println("Animate VIJF OVER HALF ... to KWART VOOR ...");
	moveWord(ClockWords[TIEN],ClockWords[KWART]);
	moveWord(ClockWords[OVER],ClockWords[VOOR2]);
	moveWord(ClockWords[HALF],ClockWords[KWART]);	
	Usb.println("KWART VOOR");
}

void inline MovingWordMatrix::Min50()
{
	// Animate KWART VOOR  ... to TIEN VOOR ...
	Usb.println("Animate KWART VOOR  ... to TIEN VOOR ...");
	moveWord(ClockWords[KWART],ClockWords[TIEN]);
	moveWord(ClockWords[VOOR2],ClockWords[VOOR]);
	Usb.println("TIEN VOOR");
}

void inline MovingWordMatrix::Min55()
{
	// Animate TIEN VOOR  ... to VIJF VOOR ...
	Usb.println("Animate TIEN VOOR  ... to VIJF VOOR ...");
	moveWord(ClockWords[TIEN],ClockWords[VIJF]);
	SetWord(ClockWords[VOOR]);
	Usb.println("VIJF VOOR");
}
  
// Hours
void inline MovingWordMatrix::Hour1()
{	
	Usb.println("TWAALF to EEN");
	moveWord(ClockWords[uTWAALF],ClockWords[uEEN]);
	Usb.println("EEN");
}

void inline MovingWordMatrix::Hour2()
{
	Usb.println("EEN to TWEE");
	moveWord(ClockWords[uEEN],ClockWords[uTWEE]);	
	Usb.println("TWEE");
}

void inline MovingWordMatrix::Hour3()
{
	Usb.println("TWEE to DRIE");
	moveWord(ClockWords[uTWEE],ClockWords[uDRIE]);
	Usb.println("DRIE");
}

void inline MovingWordMatrix::Hour4()
{
	moveWord(ClockWords[uDRIE],ClockWords[uVIER]);
	Usb.println("VIER");
}

void inline MovingWordMatrix::Hour5()
{
	moveWord(ClockWords[uVIER],ClockWords[uVIJF]);
	Usb.println("VIJF");
}

void inline MovingWordMatrix::Hour6()
{
	moveWord(ClockWords[uVIJF],ClockWords[uZES]);
	Usb.println("ZES");
}

void inline MovingWordMatrix::Hour7()
{
	moveWord(ClockWords[uZES],ClockWords[uZEVEN]);
	Usb.println("ZEVEN");
}

void inline MovingWordMatrix::Hour8()
{
	moveWord(ClockWords[uZEVEN],ClockWords[uACHT]);
	Usb.println("ACHT");
}

void inline MovingWordMatrix::Hour9()
{
	moveWord(ClockWords[uACHT],ClockWords[uNEGEN]);
	Usb.println("NEGEN");
}

void inline MovingWordMatrix::Hour10()
{
	moveWord(ClockWords[uNEGEN],ClockWords[uTIEN]);
	Usb.println("TIEN");
}

void inline MovingWordMatrix::Hour11()
{
	moveWord(ClockWords[uTIEN],ClockWords[uELF]);
	Usb.println("ELF");
}

void inline MovingWordMatrix::Hour12()
{
	moveWord(ClockWords[uELF],ClockWords[uTWAALF]);
	Usb.println("TWAALF");
}

void inline MovingWordMatrix::Plus1()
{
	SetWord(ClockWords[PLUSEEN]);
	Usb.println("PLUSEEN");
}

void inline MovingWordMatrix::Plus2()
{
	moveWord(ClockWords[PLUSEEN],ClockWords[PLUSTWEE]);	
	Usb.println("PLUSTWEE");
}

void inline MovingWordMatrix::Plus3()
{
	moveWord(ClockWords[PLUSTWEE],ClockWords[PLUSDRIE]);
	Usb.println("PLUSDRIE");
}

void inline MovingWordMatrix::Plus4()
{
	moveWord(ClockWords[PLUSDRIE],ClockWords[PLUSVIER]);	
	Usb.println("PLUSVIER");
}