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
//include your programs
#include "demos.h"
#include "Settings.h"

#define PROGS 4
#define SETPROGS 0

#define DATAPOINTS 100

RTC_Millis rtc;

menu settings;

temperatureController::thermistor EOM(A2,12);

float feedbackTime = 1000;

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
  graph plt(5*CHARW,3*CHARH); 
  const int numTraces = 4;
  bitArray traces[numTraces];
  int colors[numTraces] = {BACKGROUND, ST7735_BLUE, ST7735_ORANGE, ST7735_YELLOW};
  for(int i = 0; i < numTraces; i++){
    traces[i] = bitArray(lcd.width()-4*CHARW, lcd.height() - 4*CHARH);
  }
  
  cls();

  //lcd.setFont(&Tiny3x3a2pt7b);
  lcd.setTextColor(ST7735_GREEN);
  lcd.println("Clock Laser RAM Control");
  lcd.setTextColor(ST7735_ORANGE);
  lcd.println("Press any key for menu.");
  lcd.setTextColor(ST7735_WHITE);

  oldPrintTime = rtc.now();
  printTime();
  int i = 0;
  plt.makeAxes(traces[1]);  plt.makeGrid(traces[1]);
  while(true){
    
    if ( (rtc.now() - oldPrintTime).seconds() >= 1){
      eraseTime(); printTime();  oldPrintTime = rtc.now();
    } 

    // if( (rtc.now() - oldScanTime).seconds() >= feedbackTime){
    //   oldScanTime = rtc.now();
    //   EOMtemps[i] = EOM.getResistance();
    //   times[i] = i;

    //   char str[50];
    //   int xold, yold;
    //   xold = lcd.getCursorX();
    //   yold = lcd.getCursorY();
      
    //   sprintf(str, "% 3.1f", vmax(EOMtemps, DATAPOINTS));
    //   printLabel(0,4, str, 6);
    //   sprintf(str, "% 3.1f", vmin(EOMtemps, DATAPOINTS));
    //   printLabel(0, SCR_HEIGHT_IN_CHARS - 3, str, 6);
    //   lcd.setCursor(xold,yold);

    //   plt.drawGraph(traces[3],BACKGROUND);
    //   plt.plotData(traces[3], times, EOMtemps, DATAPOINTS);
    //   plt.drawGraph(traces[1],ST7735_BLUE);
    //   plt.drawGraph(traces[3],ST7735_YELLOW);
      
    //   i = (i + 1) % DATAPOINTS;
    // }
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
  lcd.fillRect(x1,y1,w,h,ST7735_GREEN);
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
