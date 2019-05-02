#include "input.h"

byte rowPins[ROWS] = {5, 6, 9, 10}; //connect to the row pinouts of the kpd
byte colPins[COLS] = {11,12,13}; //connect to the column pinouts of the kpd
char keys[ROWS][COLS] = {
{'1','2','3'},
{'4','5','6'},
{'7','8','9'},
{'*','0','#'}
};

Keyboard keypad = Keyboard( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

void keypad_initialize(){
	keypad.addEventListener(keypadEvent);
}

void keypadEvent(KeypadEvent key){
	switch (keypad.getState()){
		case PRESSED:
			released = false;
			hold = false;
			pressed = true;
			break;
		case HOLD:
			hold = true;
			pressed = false;
			released = false;
			break;
		case RELEASED:
			released = true;
			hold = false;
			pressed = false;
			break;
	}
}

int numInput(String prompt){
	String input;

	lcd.print(prompt);
		
	char key = '\0';
	//printCoords();
	while(true){
		key = keypad.getKey();
		switch(key){
			case '*':
				if(input.length() > 0){
					eraseChar();
					//printCoords();
					input.remove(input.length()-1);
				}
				break;
			case '#':
				if(input.length() > 0){
					lcd.println();
					return input.toInt(); 
				}
				break;
			default:
				input.concat(key);
				lcd.print(key);
				//printCoords();
			case '\0':
				break;
		}
		delay(10);
	}
}

void wait(){
	lcd.print("Press any key to continue.");
	while(true){
		delay(25);
		char key = keypad.getKey();
	
		//wait for key until pressed
		if(key)
			break;
	}
	cls();
}

void wait(String prompt){
	lcd.print(prompt);
	while(true){
		delay(25);
		char key = keypad.getKey();
	
		//wait for key until pressed
		if(key)
			break;
	}
	cls();
}

void eraseChar(){
	//assumes monospaced font!
	int16_t x1,y1,x,y;
	uint16_t charW,charH;
	x = lcd.getCursorX();
	y = lcd.getCursorY();

	lcd.getTextBounds(String("x"),x,y, &x1, &y1, &charW, &charH);

	if(x >= charW){
		lcd.fillRect(x-charW,y,charW,charH,BACKGROUND);
		lcd.setCursor(x-charW,y);
	 } else if(y >= charH) {
		int wrapAroundX = (x- charW + lcd.width() - (lcd.width() % charW) ) % lcd.width();
		lcd.fillRect(
			wrapAroundX,
			y - charH,
			charW,charH,
			BACKGROUND);
			
		lcd.setCursor(wrapAroundX,y-charH);
	 } else {
		lcd.fillRect(0,0,charW,charH,BACKGROUND);
		lcd.setCursor(0,0);
	 }
}

void eraseChar(int n){
	for(int i = 0; i < n; i++){
		eraseChar();
	}
}
