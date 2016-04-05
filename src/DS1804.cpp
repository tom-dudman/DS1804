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
#include "Arduino.h"
#include "DS1804.h"

DS1804::DS1804( byte CSpin, byte INCpin, byte UDpin, unsigned long maxResistance ) {
	m_CSpin = CSpin;
	m_INCpin = INCpin;
	m_UDpin = UDpin;
	
	pinMode( m_CSpin, OUTPUT );
	pinMode( m_INCpin, OUTPUT );
	pinMode( m_UDpin, OUTPUT );
	
	digitalWrite( m_INCpin, HIGH );
	
	m_maxResistance = maxResistance;
	m_steps = 99;
	
	m_wiperPosition = 0;
}

DS1804::~DS1804() {
	digitalWrite( m_CSpin, LOW );
	digitalWrite( m_INCpin, LOW );
	digitalWrite( m_UDpin, LOW );
}

/* READER FUNCTIONS

	isLocked()
	returns the state of the Chip Select pin
	
	getWiperPosition()
	returns the wiper position ( between 0 and m_steps )
	
	getResistance()
	calculates and returns the resistance from the wiper position

*/

bool DS1804::isLocked() {
	return digitalRead( m_CSpin );
}

byte DS1804::getWiperPosition() {
	return m_wiperPosition;
}

unsigned long DS1804::getResistance() {
	return wiperPositionToResistance( getWiperPosition() );
}

/*	HELPER FUNCTIONS

	resistanceToWiperPosition( unsigned long wantedResistance )
	calculates and returns the wiper position for any required resistance
	
	wiperPositionToResistance( byte wantedWiperPosition )
	calculates and returns the resistance at a given wiper position
	
	resistanceToActualResistance( unsigned long wantedResistance )
	calculates and returns the nearest available resistance to the wantedResistance

*/

byte DS1804::resistanceToWiperPosition( unsigned long wantedResistance ) {
	// translate resistance to wiper position
	unsigned long newResistance = constrain( wantedResistance, 0, m_maxResistance );
	return map( newResistance, 0, m_maxResistance, 0, m_steps );
}

unsigned long DS1804::wiperPositionToResistance( byte proposedWiperPosition ) {
	// work out the resistance from a wiper position
	byte newWiperPosition = constrain( proposedWiperPosition, 0, m_steps );
	return map( newWiperPosition, 0, m_steps, 0, m_maxResistance );
}

unsigned long DS1804::resistanceToActualResistance( unsigned long wantedResistance ) {
	// work out the actual resistance
	return wiperPositionToResistance( resistanceToWiperPosition( wantedResistance ) );
}

/*	SETTER FUNCTIONS

	lock()
	sets the Chip Select pin HIGH to block any changes to wiper position
	
	unlock()
	sets the Chip Select pin LOW to allow changes to wiper position
	
	write()
	sets the INC pin HIGH, and 
	
	setToZero()
	sets the wiper position to zero from any previous position
	you may want to call this immediately after instantiation to erase any previous setting

	setWiperPosition( byte WiperPosition )
	sets and returns the wiper position to the required position
	
	overrideWiperPosition( byte WiperPosition )
	sets and returns the wiper position of your program object WITHOUT actually changing the chip
	useful if you know the value on the chip, and want your model to match it
	
	setResistance( unsigned long wantedResistance )
	sets and returns the resistance to the nearest possible resistance
*/

void DS1804::lock() {
	digitalWrite( m_CSpin, HIGH );
	delayMicroseconds(50);
}

void DS1804::unlock() {
	digitalWrite( m_CSpin, LOW );
	delayMicroseconds( CS_TO_INC_SETUP );
}

void DS1804::write() {
	digitalWrite( m_CSpin, LOW );
	digitalWrite( m_INCpin, HIGH );
	digitalWrite( m_CSpin, HIGH );
	delayMicroseconds( WIPER_STORAGE_TIME );
	digitalWrite( m_CSpin, LOW );
	delayMicroseconds( WIPER_STORAGE_TIME - CS_DESELECT_TIME );
}

void DS1804::setToZero() {
	digitalWrite( m_UDpin, LOW );
	delayMicroseconds(1);
	transmitPulses(100);
}

byte DS1804::setWiperPosition( byte wiperPosition ) {
	if ( !isLocked() ) {
		
		// work out the number of pulses to get to new position
		byte newWiperPosition = constrain( wiperPosition, 0, m_steps );
		byte pulses = abs( newWiperPosition - m_wiperPosition );
		
		// set the UD pin accordingly
		digitalWrite( m_UDpin, newWiperPosition > m_wiperPosition );
		delayMicroseconds(1);
		
		// send the INC pulses
		transmitPulses( pulses );
		
		m_wiperPosition = newWiperPosition;
	}
	return m_wiperPosition;	
}

byte DS1804::overrideWiperPosition( byte wiperPosition ) {
	
	return m_wiperPosition = constrain( wiperPosition, 0, m_steps );
}

unsigned long DS1804::setResistance( unsigned long wantedResistance ) {
	
	if ( !isLocked() ) {
		
		setWiperPosition( resistanceToWiperPosition( wantedResistance ) );
		
	}
	return getResistance();
}

void DS1804::transmitPulses( byte wantedPulses ) {
	byte pules = constrain( wantedPulses, 0, m_steps );
	digitalWrite( m_INCpin, HIGH );
	delayMicroseconds(1);
	for( byte n=0; n<wantedPulses; n++ ) {
		digitalWrite( m_INCpin, LOW );
		delayMicroseconds(1);
		digitalWrite( m_INCpin, HIGH );
		delayMicroseconds(1);
	}
}