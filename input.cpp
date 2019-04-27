#include "input.h"

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

void keypadEvent(KeypadEvent key){
  switch (keypad.getState()){
    default:
      break;
    //pass;
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
        lcd.println();
        return input.toInt(); 
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
   if(lcd.getCursorX() >= CHARW){
    lcd.fillRect(lcd.getCursorX()-CHARW,lcd.getCursorY(),CHARW,CHARH,BACKGROUND);
    lcd.setCursor(lcd.getCursorX()-CHARW,lcd.getCursorY());
   } else if(lcd.getCursorY() >= CHARH) {
    int wrapAroundX = (lcd.getCursorX()- CHARW + lcd.width() - (lcd.width() % CHARW) ) % lcd.width();
    lcd.fillRect(
      wrapAroundX,
      lcd.getCursorY() - CHARH,
      CHARW,CHARH,
      BACKGROUND);
      
    lcd.setCursor(wrapAroundX,lcd.getCursorY()-CHARH);
   } else {
    lcd.fillRect(0,0,CHARW,CHARH,BACKGROUND);
    lcd.setCursor(0,0);
   }
}

void eraseChar(int n){
  for(int i = 0; i < n; i++){
    eraseChar();
  }
}
