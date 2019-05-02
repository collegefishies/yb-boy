/* @file MultiKey.ino
|| @version 1.0
|| @author Mark Stanley
|| @contact mstanley@technologist.com
||
|| @description
|| | The latest version, 3.0, of the keypad library supports up to 10
|| | active keys all being pressed at the same time. This sketch is an
|| | example of how you can get multiple key presses from a keypad or
|| | keyboard.
|| #
*/
#include "input.h"


unsigned long loopCount;
unsigned long startTime;
String msg;


void setup() {
	lcd_initialize();
	Serial.begin(9600);
	loopCount = 0;
	startTime = millis();
	msg = "";
}


void loop() {
	String input = numInput("Enter number: ");
	if(input.length() == 0){
		lcd.println("No input.");
	} else {
		lcd.print("You input: ");
		char buff[10];
		sprintf(buff, "%e",input.toFloat());
		lcd.println(buff);
	}

	wait();
	cls();


}  // End loop