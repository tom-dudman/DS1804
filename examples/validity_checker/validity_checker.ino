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
