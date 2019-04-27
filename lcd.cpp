#include "lcd.h"

Adafruit_ST7735 lcd = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

void lcd_initialize(){
  lcd.initR(INITR_BLACKTAB);
  lcd.setTextWrap(true);
  lcd.setRotation(3);
}


void info(){
  lcd.setCursor(0,0);
  lcd.println("YbBoy (c) 2019");
}

void cls(){
  lcd.setCursor(0,0);
  lcd.fillScreen(BACKGROUND);
}

void printCoords(){
  int x0,y0;
  x0 = lcd.getCursorX();
  y0 = lcd.getCursorY();

  //erase bar
  lcd.fillRect(0,lcd.height()-CHARH,lcd.width(),CHARH, BACKGROUND);
  
  //print info
  lcd.setCursor(0,lcd.height()-CHARH);
  lcd.print("X="); lcd.print(x0);
  lcd.print(" Y="); lcd.print(y0);

  //return
  lcd.setCursor(x0,y0);
}
