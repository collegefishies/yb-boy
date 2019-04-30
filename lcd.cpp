#include "lcd.h"

myLCD lcd(TFT_CS, TFT_DC, TFT_RST);

void myLCD::colorPrint(String str, int temp_color){
  int old_color = textcolor;
  lcd.setTextColor(temp_color);
  lcd.print(str);
  lcd.setTextColor(old_color);
}


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
