// Moonlite-compatible stepper controller
//
// Uses AccelStepper (http://www.airspayce.com/mikem/arduino/AccelStepper/)
//
// Inspired by (http://orlygoingthirty.blogspot.co.nz/2014/04/arduino-based-motor-focuser-controller.html)
// orly.andico@gmail.com, 13 April 2014
//
// Modified for indilib, easydriver by Cees Lensink

// Hacked by WvB
// To be used with geared motor
// This sketch works very basic.



#include "GearedMotor.h"

/*
//WvB
int stepperPin = 2;
int dirPin = 8;
int powerPin = 4;
boolean useSleep = true; // true= use sleep pin, false = use enable pin
//WvB
*/
int powerPin = 9;
boolean useSleep = true; // true= use sleep pin, false = use enable pin

// maximum speed is 160pps which should be OK for most
// tin can steppers
#define MAXSPEED 160
#define SPEEDMULT 3

GearedMotor motor(9,8,7);

#define MAXCOMMAND 8

char inChar;
char cmd[MAXCOMMAND];
char param[MAXCOMMAND];
char line[MAXCOMMAND];
long pos;
int eoc = 0;
int idx = 0;
boolean isRunning = false;

char tempString[10];


void setup()
{
	Serial.begin(9600);
	pinMode(powerPin,OUTPUT);
	//wvb motor.setSpeed(MAXSPEED);
	//wvb motor.setMaxSpeed(MAXSPEED);
	//wvb motor.setAcceleration(50);
	//WvB new code starts here
	//WvB Skywatcher motor has backlash. About 120 - 150 steps will do. Check if this has effect (haven't tried it yet)
	//WvB Assume focuser starts at all the way in, which is position 0
	motor.setCurrentPosition(0);
	motor.setBacklashFlag(true);
	motor.setBacklashSteps(120);
	//WvB new code ends here
	turnOff();
	memset(line, 0, MAXCOMMAND);
}


//

//


void loop(){
	if (isRunning) { // only have to do this is stepper is on
		motor.run();
		if (motor.stepsToGo() == 0) {
			//  we have arrived, remove power from motor
			turnOff();
		}
	}

	// read the command until the terminating # character
	//WvB each command starts with : and ends with #
	//WvB the following seems strange, as idx is never set to 0
	//WvB this code will put all characters received between : and # in the string line
	while (Serial.available() && !eoc) {
		inChar = Serial.read();
		if (inChar != '#' && inChar != ':') {
			line[idx++] = inChar;
			if (idx >= MAXCOMMAND) {
				idx = MAXCOMMAND - 1;
			}
		}
		else {
			if (inChar == '#') {
				eoc = 1;
			}
		}
	} // end while Serial.available()
	// we may not have a complete command yet but there is no character coming in for now and might as well loop in case stepper needs updating
	// eoc will flag if a full command is there to act upon

	// process the command we got
	if (eoc) {
		memset(cmd, 0, MAXCOMMAND);
		memset(param, 0, MAXCOMMAND);

		int len = strlen(line);
		if (len >= 2) {
			strncpy(cmd, line, 2);
		}

		if (len > 2) {
			strncpy(param, line + 2, len - 2);
		}

		memset(line, 0, MAXCOMMAND);  //WvB sets the string line to 0
		eoc = 0;
		idx = 0;

		//now execute the command

		//Immediately stop any focus motor movement. returns nothing
		//code from Quickstop example. This is blocking
		if (!strcasecmp(cmd, "FQ")) {  // WvB: "FQ" = Halt motor, position is retained
			//WvB turnOn();
			motor.disableOutputs(); // Stop as fast as possible: sets new target
			pos = motor.currentPosition();
			//WvB motor.moveTo(0);  // WvB: is this needed for dc?
			// Now stopped after quickstop
		}

		//Go to the new position as set by the ":SNYYYY#" command. returns nothing    // initiate a move
		//turn stepper on and flag it is running
		// is this the only command that should actually make the stepper run ?
		if (!strcasecmp(cmd, "FG")) {  //WvB: start motor, to NewPosition
			turnOn();
		}

		//Returns the temperature coefficient where XX is a two-digit signed (2’s complement) hex number.
		//hardcoded
		if (!strcasecmp(cmd, "GC")) {
			Serial.print("02#");
		}

		//Returns the current stepping delay where XX is a two-digit unsigned hex number. See the :SD# command for a list of possible return values.
		//hardcoded for now
		// might turn this into AccelStepper acceleration at some point
		if (!strcasecmp(cmd, "GD")) {
			Serial.print("02#");
		}

		//Returns "FF#" if the focus motor is half-stepped otherwise return "00#"
		//hardcoded
		if (!strcasecmp(cmd, "GH")) {
			Serial.print("00#");
		}

		//Returns "00#" if the focus motor is not moving, otherwise return "01#",
		//AccelStepper returns Positive as clockwise
		if (!strcasecmp(cmd, "GI")) {
			if (motor.stepsToGo() == 0) {
				Serial.print("00#");
			}
			else {
				Serial.print("01#");
			}
		}

		//Returns the new position previously set by a ":SNYYYY" command where YYYY is a four-digit unsigned hex number.
		if (!strcasecmp(cmd, "GN")) {
			pos = motor.targetPosition();
			sprintf(tempString, "%04X", pos);
			Serial.print(tempString);
			Serial.print("#");
		}

		//Returns the current position where YYYY is a four-digit unsigned hex number.
		if (!strcasecmp(cmd, "GP")) {
			pos = motor.currentPosition();
			sprintf(tempString, "%04X", pos);
			Serial.print(tempString);
			Serial.print("#");
		}

		//Returns the current temperature where YYYY is a four-digit signed (2’s complement) hex number.
		if (!strcasecmp(cmd, "GT")) {
			Serial.print("0020#");
		}

		//Get the version of the firmware as a two-digit decimal number where the first digit is the major version number, and the second digit is the minor version number.
		//hardcoded
		if (!strcasecmp(cmd, "GV")) {
			Serial.print("10#");
		}

		//Set the new temperature coefficient where XX is a two-digit, signed (2’s complement) hex number.
		if (!strcasecmp(cmd, "SC")) {
			//do nothing yet
		}

		//Set the new stepping delay where XX is a two-digit,unsigned hex number.
		if (!strcasecmp(cmd, "SD")) {
			//do nothing yet
		}

		//Set full-step mode.
		if (!strcasecmp(cmd, "SF")) {
			//do nothing yet
		}

		//Set half-step mode.
		if (!strcasecmp(cmd, "SH")) {
			//do nothing yet
		}

		//Set the new position where YYYY is a four-digit
		if (!strcasecmp(cmd, "SN")) {
			pos = hexstr2long(param);
			// stepper.enableOutputs(); // turn the motor on here ??
			//WvB: this seems strange, motor is turned on here. There should be another command to do this
			turnOn();
			motor.moveTo(pos);
		}

		//Set the current position where YYYY is a four-digit unsigned hex number.
		if (!strcasecmp(cmd, "SP")) {
			pos = hexstr2long(param);
			motor.setCurrentPosition(pos);
		}

	}// end if(eoc)


} // end loop

long hexstr2long(char *line) {
	long ret = 0;

	ret = strtol(line, NULL, 16);
	return (ret);
}

void turnOn() {
	if (useSleep) {
		digitalWrite(powerPin, HIGH);
		} else {
		digitalWrite(powerPin, LOW);
	}
	isRunning = true;
}
void turnOff() {
	if (useSleep) {
		digitalWrite(powerPin, LOW);
		} else {
		digitalWrite(powerPin, HIGH);
	}
	isRunning = false;
}

