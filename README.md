# Arduino.DS1804

An Arduino library to control a DS1804.  The DS1804 is a programmable linear potentiometer IC, with 100 positions.  There are 10 k, 50 k and 100 k variants.

The library controls the position of the wiper on the DS1804, and allows it to be locked and unlocked, and written to the IC's EEPROM.

The wiper position can be set directly, or a resistance can be requested and the appropriate wiper position is set.

A number of helper functions are provided to model the characteristics of the DS1804 without having to actually set it first.

A data sheet for DS1804 can be downloaded from here: http://datasheets.maximintegrated.com/en/ds/DS1804.pdf
If this link is broken, a local copy is [here](/extras/DS1804.pdf).

## Installation

For a guide to installing Arduino libraries, see https://www.arduino.cc/en/Guide/Libraries

## Using in a project

After installing the library, to use it in a sketch, in the Arduino IDE choose **Sketch > Import Library**.

## Functions

### Constructor

    DS1804( byte CSpin, byte INCpin, byte UDpin, unsigned long maxResistance );

This function creates a new instance of the DS1804 class that represents a particular DS1804 chip attached to your Arduino board.

#### Parameters

`CSpin, INCpin, UDpin`: the Arduino pins that are attached to the Chip Select (7), Increment (1) and Up/Down Control (2) pins respectively (*byte*)

`maxResistance`: the maximum resistance of the chip.  The part no (see table below) can be passed as this argument, or an integer (*unsigned long*)

#### Part Nos

The DS1804 is available in three models in three packages

Part No | Max Resistance (ohms) | Package
------- | -------------------: | -------
DS1804_TEN | 10 000 | 8-pin DIP
DS1804Z_TEN | 10 000 | 8-pin SO
DS1804U_TEN | 10 000 | 8-pin μSOP
DS1804_FIFTY | 50 000 | 8-pin DIP
DS1804Z_FIFTY | 50 000 | 8-pin SO
DS1804U_FIFTY | 50 000 | 8-pin μSOP
DS1804_HUNDRED | 100 000 | 8-pin DIP
DS1804Z_HUNDRED | 100 000 | 8-pin SO
DS1804U_HUNDRED | 100 000 | 8-pin μSOP


### Getter Functions

`isLocked()`: returns `true` if the DS1804 is locked, or `false` if not

`getResistance()`: returns the current actual resistance setting of the DS1804

`getWiperPosition()`: returns the current wiper position of the DS1804

### Helper Functions

Please note that these functions assume an ideal component, with evenly spaced wiper positions.  You may wish to build a lookup table for your specific component if greater accuracy is required.

`resistanceToWiperPosition( unsigned long wantedResistance )`: returns the wiper position that most closely matches the *wantedResistance*.

`wiperPositionToResistance( byte wiperPosition )`: returns the resistance that would be achieved at *wiperPosition* in *ohms*

`resistanceToActualResistance( unsigned long wantedResistance )`: returns the closest possible resistance to the *wanted resistance* in *ohms*

### Setter Functions

`lock()`: locks the DS1804 at the current wiper position

`unlock()`: unlocks the DS1804, so the wiper position can be changed

`write()`: writes the current wiper position to the DS1804's EEPROM

`setToZero()`: sets the wiper position to zero (especially useful if the chip's EEPROM is worn-out)

`setWiperPosition( byte wiperPosition )`: set the wiper to a specified position, returns the wiper position

`overrideWiperPosition( byte wiperPosition )`: sets and returns the wiper position of your program object WITHOUT actually changing the chip

`setResistance( unsigned long resistance )`: set the resistance to the closest possible resistance to a specified resistance, returns the actual resistance

#### A note about writing to EEPROM

If the wiper position is written to the EEPROM, it will be restored the next time the DS1804 is powered up.

The EEPROM has a lifetime of 50000 writes.  These are the approximate life times for given write rates.

Write rate | Life time
---------- | ---------
1/day | 137 years
1/hour | 5.7 years
1/minute | 34.7 days
1/second | <14 hours
1/millisecond | 50 seconds


## Examples

![DS1804 Arduino Circuit](/images/DS1804_circuit.png)

The green wire is only used in example 1.  The push switch is only used in example 4.

### 1. Serial Control

Allow the user to control a DS1804 via a Serial connection, and use the analogue pins to read the voltage at the wiper and send the reading back to the computer.

```Arduino

#include <DS1804.h>
	
const byte wiperPin = A5;
DS1804 digipot = DS1804( 6, 5, 4, DS1804_TEN );
	
byte reading = 0;
unsigned long new_resistance = 0;
	
void setup() {
  Serial.begin(9600);
  digipot.setToZero();
}
	
void loop() {
  if ( Serial.available() > 0 ) {
    new_resistance = Serial.parseFloat();
    digipot.setResistance( new_resistance );
    delay(1);	// give DS1804 time to change, and for analogue reading to settle
    reading = analogRead( wiperPin );
    String wiperPositionText = "DS1804 Wiper at ";
    Serial.println( wiperPositionText + digipot.getWiperPosition() );
    String resistanceText = "DS1804 Resistance set to ";
    Serial.println( resistanceText + digipot.getResistance() + " ohms (requested " + new_resistance + " ohms)" );
    String wiperPinVoltageText = "Wiper pin voltage: ";
    Serial.println( wiperPinVoltageText + 4.9*reading + " mV" );
  }
}
```
	
### 2. Validity Checker

Check whether a requested resistance is possible, and set if so.

```Arduino
#include <DS1804.h>
	
DS1804 myTrimmer = DS1804( 6, 5, 4, DS1804_TEN );
	
unsigned long myTrimmerResistance = 0;
	
unsigned long setResistanceIfPossible( DS1804 &trimmer, unsigned long resistance ) {
	if ( trimmer.resistanceToActualResistance( resistance ) == resistance ) {
		trimmer.setResistance( resistance );
	}
	return trimmer.getResistance();
}

void setup() {
	myTrimmer.setToZero();
	
	myTrimmerResistance = setResistanceIfPossible( myTrimmer, 1000 );	// possible!	myTrimmerResistance == 1000;
	myTrimmerResistance = setResistanceIfPossible( myTrimmer, 108.5 );	// impossible!	myTrimmerResistance == 1000;
	myTrimmerResistance = setResistanceIfPossible( myTrimmer, 5000 );	// possible!	myTrimmerResistance == 5000;
}

void loop() {}
```

### 3. Rate Limited Potentiometer

Prevent a potentiometer being changed too often.

```Arduino
#include <DS1804.h>
	
DS1804 rateLimitedDigipot = DS1804( 6, 5, 4, DS1804_FIFTY );
unsigned long rateLimit = 5000;	// time in ms required between changes

long lastLockedAt = -1*rateLimit;
byte lastWiperPosition;

void setup() {
  rateLimitedDigipot.setToZero();
  lastWiperPosition = rateLimitedDigipot.getWiperPosition();
  Serial.begin(9600);
}

void loop() {

	if ( rateLimitedDigipot.isLocked() ) {
		// unlock if enough time has passed		
		if ( ( millis() - lastLockedAt ) > rateLimit ) {
			rateLimitedDigipot.unlock();
		}
	}
	else {
	// detect change and lock		
		if ( lastWiperPosition != rateLimitedDigipot.getWiperPosition() ) {
			lastWiperPosition = rateLimitedDigipot.getWiperPosition();
			rateLimitedDigipot.lock();
			lastLockedAt = millis();
		}
	}
	
	if ( Serial.available() > 0 ) {
      rateLimitedDigipot.setResistance( Serial.parseFloat() );
	}	
}
```

### 4. Non-volatile Potentiometer

Assign and store a wiper position in both the DS1804's and Arduino's EEPROM, and restore it if the program restarts.

```Arduino
#include <EEPROM.h>
#include <DS1804.h>

DS1804 digipot = DS1804( 6, 5, 4, DS1804_HUNDRED );

const byte _buttonPin = 12;
const int _addr = 0;

void setup() {

	byte restoredValue = EEPROM.read( _addr );
	
	// check if the EEPROM has been written before
	if ( restoredValue != 255 ) {		
		restoredValue = constrain( restoredValue, 0, 99 );
		digipot.overrideWiperPosition( restoredValue );
	}
	else {
		setRandomWiperPosition();
	}
	
	pinMode( _buttonPin, INPUT_PULLUP );
}

void loop() {

	// wait for button to be pushed	
	do {} while ( digitalRead( _buttonPin ) == HIGH );
	
	setRandomWiperPosition();
	
	// don't hold the button for more than a second to avoid a rewrite!
	delay(1000);
}

void setRandomWiperPosition() {

	byte newPosition = random( 100 );
	
	newPosition = digipot.setWiperPosition( newPosition );
	digipot.write();
	
	if ( EEPROM.read( _addr ) != newPosition ) {
          EEPROM.write( _addr, newPosition );
        }
}
```