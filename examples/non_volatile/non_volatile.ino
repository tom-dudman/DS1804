#include <EEPROM.h>
#include <DS1804.h>

DS1804 digipot = DS1804( 6, 5, 4, DS1804_100 );

const byte _buttonPin = 12;
const int _addr = 0;

void setup() {

	byte restoredValue = EEPROM.read( _addr );
	
	// check if the EEPROM has been written before
	if ( restoredValue !== 255 ) {		
		restoredValue = constrain( restoredValue, 0, 99 );
		digipot.overrideWiperPosition( restoredValue );
	}
	else {
		setRandomWiperPosition();
	}
	
	pinMode( _pin, INPUT_PULLUP );
}

void main() {

	// wait for button to be pushed	
	do {} while ( digitalRead( _buttonPin ) == HIGH )
	
	setRandomWiperPosition();
	
	// don't hold the button for more than a second to avoid a rewrite!
	delay(1000);
}

void setRandomWiperPosition() {

	byte newPosition = random( 100 );
	
	newPosition = digipot.setWiperPosition( newPosition );
	digipot.write();
	
	EEPROM.update( _addr, newPosition );
}
