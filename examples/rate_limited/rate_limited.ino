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
