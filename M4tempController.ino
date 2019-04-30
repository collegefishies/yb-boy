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
#include "temperatureController.h"
#include <RTClib.h>
#include <math.h>
#include "Fonts/Tiny3x3a2pt7b.h"

#define PROGS 4
#define SETPROGS 0

#define DATAPOINTS 100

RTC_Millis rtc;

menu settings;

temperatureController::thermistor EOM(A2,12);

float feedbackTime = 1;

float EOMtemps[DATAPOINTS] = {0};
float FiberErrors[DATAPOINTS] = {0};
float times[DATAPOINTS] = {0};

String settingsItems[SETPROGS]   = {};
void (*settingsProgs[SETPROGS])()= {};

void printTime();

float vmax( float* x, int len);
float vmin( float* x, int len);
void printLabel(int x, int y, char* str, int len);

void setup() {
  rtc.begin(DateTime(2019,1,1,12,0,0));
	lcd_initialize();
	
	settings.defineTitle("-------Settings------");
	settings.defineMenuItems(settingsItems,SETPROGS);
	settings.defineMenuProgs(settingsProgs,SETPROGS);

  EOM.setVoltage(3.3); 
  EOM.setResistorDivider(2.49e3);
}


DateTime oldPrintTime;
DateTime oldScanTime;

void loop() {
  graph plt(lcd.width()/2,0,lcd.width(),lcd.height()/2);   
  cls();

  lcd.setFont(&Tiny3x3a2pt7b);
  lcd.setTextColor(ST7735_GREEN);
  lcd.println("Clock Laser RAM Control");
  lcd.setTextColor(ST7735_ORANGE);
  lcd.println("Press any key for menu.");
  lcd.setTextColor(ST7735_WHITE);

  oldPrintTime = rtc.now();
  printTime();
  int i = 0;

  // plt.setXlabels(false);
  plt.setBoundary(10);
  plt.setYlims(20,70);
  plt.setXlabels(false);

  plt.makeAxes();
  plt.makeGrid();
  while(true){
    
    if ( (rtc.now() - oldPrintTime).seconds() >= 1){
      // eraseTime(); printTime();  oldPrintTime = rtc.now();
    } 

    if( (rtc.now() - oldScanTime).seconds() >= feedbackTime){
      oldScanTime = rtc.now();
      EOMtemps[i] = EOM.getResistance();
      times[i] = i;

      plt.plotData(0, times, EOMtemps, DATAPOINTS);
      plt.drawGraph();
      
      i = (i + 1) % DATAPOINTS;
    }
  }
	//settings.ui();
}

void printTime(){
  DateTime now = rtc.now();

  char str[50];
  sprintf(str,"%02d:%02d:%02d %02d/%02d/%04d",now.hour(),now.minute(),now.second(),now.month(), now.day(), now.year());
  lcd.print(str);
}

void eraseTime(){
  DateTime now = rtc.now();

  char str[20];
  sprintf(str,"%02d:%02d:%02d %02d/%02d/%04d",now.hour(),now.minute(),now.second(),now.month(), now.day(), now.year());
  
  int x,y;

  x = lcd.getCursorX();
  y = lcd.getCursorY();

  int16_t x1,y1;
  uint16_t w,h;

  lcd.getTextBounds(String(str), x, y, &x1, &y1, &w,&h);
  lcd.fillRect(x1,y1,w,h,BACKGROUND);
  lcd.setCursor(x1, y1);  
}

float vmax( float* x, int len){
  float sup = x[0];
  for(int i = 0; i < len; i++){
    sup = max(sup,x[i]);
  }
  return sup;
}

float vmin( float* x, int len){
  float sup = x[0];
  for(int i = 0; i < len; i++){
    sup = min(sup,x[i]);
  }
  return sup;
}

void printLabel(int x, int y, char* str, int len){
      lcd.setCursor((x+len)*CHARW,y*CHARH);
      eraseChar(len);
      lcd.print(str);
}
