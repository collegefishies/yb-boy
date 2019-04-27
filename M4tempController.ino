/*  menu.ino --- this is a menu cpp
 *  ===============================
 *  this is intended to be the top 
 *  level file. add functions via .h
 *  files.
 * 
 *  To add your own custom function just 
 *  add a title to menuItems, increase PROGS by 1
 *  and add your void foo() function to menuProgs.
 */


//include your LCD libraries
#include "lcd.h"    //includes lcd class for screen communication
#include "input.h"  //includes keypad inputs
#include "menu.h"

#include "bitArray.h"
#include "graph.h"
#include <math.h>


void printLabel(int x, int y, char* str, int len);

void setup() {
	lcd_initialize();
  cls();
  graph test(0,0,lcd.width(),lcd.height());
  test.setBoundary(1);
  test.setXtics(3);
  test.setYtics(5);
  test.makeAxes();
  test.makeGrid();
  test.drawGraph();
}


void loop() {

}


void printLabel(int x, int y, char* str, int len){
      lcd.setCursor((x+len)*CHARW,y*CHARH);
      eraseChar(len);
      lcd.print(str);
}
