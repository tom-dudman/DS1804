/*
	DS1804.cpp - a library for controlling a DS1804 Nonvolatile Trimmer Potentiometer
	http://datasheets.maximintegrated.com/en/ds/DS1804.pdf
	
	The MIT License (MIT)

	Copyright (c) 2016 tom-dudman

	Permission is hereby granted, free of charge, to any person obtaining a copy
	of this software and associated documentation files (the "Software"), to deal
	in the Software without restriction, including without limitation the rights
	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the Software is
	furnished to do so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included in all
	copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
	SOFTWARE.
*/
#ifndef DS1804_h
#define DS1804_h

#include "Arduino.h"

#define CS_TO_INC_SETUP 1
#define CS_DESELECT_TIME 1
#define WIPER_STORAGE_TIME 10000

#define DS1804_TEN 10000
#define DS1804Z_TEN 10000
#define DS1804U_TEN 10000
#define DS1804_FIFTY 50000
#define DS1804Z_FIFTY 50000
#define DS1804U_FIFTY 50000
#define DS1804_HUNDRED 100000
#define DS1804Z_HUNDRED 100000
#define DS1804U_HUNDRED 100000

class DS1804 {
	
	public:
		
		DS1804( byte CSpin, byte INCpin, byte UDpin, unsigned long maxResistance );
		~DS1804();
		
		// getter functions
		bool isLocked();
		unsigned long getResistance();
		byte getWiperPosition();
		
		// helpful mapper functions
		byte resistanceToWiperPosition( unsigned long wantedResistance );
		unsigned long wiperPositionToResistance( byte wantedWiperPosition );
		unsigned long resistanceToActualResistance( unsigned long wantedResistance );
		
		// setter functions
		void lock();
		void unlock();
		void write();
		void setToZero();
		byte setWiperPosition( byte wiperPosition );
		byte overrideWiperPosition( byte wiperPosition );
		unsigned long setResistance( unsigned long resistance );
		
	private:
		
		// pins
		byte m_CSpin;	// the Chip Select pin
		byte m_INCpin;	// the Increment/Decrement Wiper Control pin
		byte m_UDpin;		// the Up/Down Control pin
		
		// physical properties
		unsigned long m_maxResistance;	// the maximum resistance in ohms (10000, 50000 or  100000)
		byte m_steps;	// the number of steps along the wipe (current models are 100)
		
		// runtime properties
		byte m_wiperPosition;
		
		// internal function
		void transmitPulses( byte pulses );
};

#endif