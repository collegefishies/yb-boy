#include "input.h"
byte ledPin; 

boolean blink;
boolean ledPin_state;

byte rowPins[ROWS] = {5, 6, 9, 10}; //connect to the row pinouts of the kpd
byte colPins[COLS] = {11,12,13}; //connect to the column pinouts of the kpd
char keys[ROWS][COLS] = {
{'1','2','3'},
{'4','5','6'},
{'7','8','9'},
{'*','0','#'}
};

Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

void keypad_initialize(){
	keypad.addEventListener(keypadEvent);
}

// Taking care of some special events.
void keypadEvent(KeypadEvent key){
    switch (keypad.getState()){
    case PRESSED:
        if (key == '#') {
            digitalWrite(ledPin,!digitalRead(ledPin));
            ledPin_state = digitalRead(ledPin);        // Remember LED state, lit or unlit.
        }
        break;

    case RELEASED:
        if (key == '*') {
            digitalWrite(ledPin,ledPin_state);    // Restore LED state from before it started blinking.
            blink = false;
        }
        break;

    case HOLD:
        if (key == '*') {
            blink = true;    // Blink the LED when holding the * key.
        }
        break;
    }
}

String numInput(String prompt){
	String input;

	lcd.setTextColor(ST7735_BLUE);
	lcd.println("Hold '1' for '.'\n     '0' for '-',\n     '3' for 'e'. ");
	lcd.println("Leave blank for no input.");
	lcd.setTextColor(ST7735_GREEN);
	lcd.print(prompt);
	lcd.setTextColor(ST7735_WHITE);
		

	while(true){
		if (keypad.getKeys())
		{
			for (int i=0; i<LIST_MAX; i++)   // Scan the whole key list.
			{
				if ( keypad.key[i].stateChanged )   // Only find keys that have changed state.
				{
					switch (keypad.key[i].kstate) {  // Report active key state : IDLE, PRESSED, HOLD, or RELEASED
						case PRESSED:
							if(keypad.key[i].kchar != '*' && keypad.key[i].kchar != '#'){
								input.concat(keypad.key[i].kchar);
								lcd.print(keypad.key[i].kchar);
							} else if (keypad.key[i].kchar == '*') {
								if(input.length() > 0){
									eraseChar();
									//printCoords();
									input.remove(input.length()-1);
								}
							} else if (keypad.key[i].kchar == '#') {
								lcd.println();
								return input; 
							}
							break;
						case HOLD:
							if(keypad.key[i].kchar == '1'){
								input.remove(input.length() - 1);
								eraseChar();
								input.concat('.');
								lcd.print('.');
							} else if (keypad.key[i].kchar == '0'){
								input.remove(input.length() - 1);
								eraseChar();
								input.concat('-');
								lcd.print('-');
							} else if (keypad.key[i].kchar == '3'){
								input.remove(input.length() - 1);
								eraseChar();
								input.concat('e');
								lcd.print('e');
							}
							break;
						case RELEASED:
							break;
						case IDLE:
							break;
					}
				}
			}
		}
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
