#ifndef INPUT_H
#define INPUT_H
#include "lcd.h"
#include <Keypad.h>

const byte ROWS = 4; //four rows
const byte COLS = 3; //three columns
extern char keys[ROWS][COLS];

//pin 33 corresponds to *0# row

extern byte rowPins[ROWS]; //connect to the row pinouts of the kpd
extern byte colPins[COLS]; //connect to the column pinouts of the kpd


class Keyboard : public Keypad{
	void init();
	bool held;
	bool released;
	bool pressed;
};

void keypadEvent(KeypadEvent key);

void eraseChar();
void eraseChar(int n);

int numInput(String prompt);  

void wait(); //wait until keypress
void wait(String prompt); //wait until keypress

extern Keypad keypad;

#endif
