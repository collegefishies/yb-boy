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
// #include "Fonts/Picopixel.h"

#define PROGS 0
#define SETPROGS 2

#define DATAPOINTS 100

RTC_Millis rtc;

menu settings;

temperatureController::thermistor EOM(A2,12);
graph plt;

float feedbackTime = 1;

float EOMtemps[DATAPOINTS] = {0};
float FiberErrors[DATAPOINTS] = {0};
float times[DATAPOINTS] = {0};


void printTime();
void promptYlims();
void promptXlims();

float vmax( float* x, int len);
float vmin( float* x, int len);
void printLabel(int x, int y, char* str, int len);
void printHeader();

String settingsItems[SETPROGS]   = {"Change graph X limits", "Change graph Y limits"};
void (*settingsProgs[SETPROGS])()= {promptXlims, promptYlims};


void setup() {
	rtc.begin(DateTime(2019,1,1,12,0,0));
	
	lcd_initialize(); 
	
	settings.defineTitle("-------PI BOX------");
	settings.defineMenuItems(settingsItems,SETPROGS);
	settings.defineMenuProgs(settingsProgs,SETPROGS);

	EOM.setVoltage(3.3); 
	EOM.setResistorDivider(2.49e3);
}


DateTime oldPrintTime;
DateTime oldScanTime;

void loop() {
	plt = graph(0,3*CHARH,lcd.width(),lcd.height()); 
	const int numTraces = 4;
	
	lcd.setFont(&Picopixel);
	lcd.setFont(&Picopixel);
	lcd.setFont(&Picopixel);
	cls();
	printHeader();

	oldPrintTime = rtc.now();
	printTime();
	int i = 0;

	plt.setBoundary(10);
	plt.setYlims(20,70);
	plt.setXauto();

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

		char key = keypad.getKey();

		if(key){
			lcd.setFont();
			settings.ui();
			cls();
			printHeader();
		}
	}
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

void printHeader(){
	lcd.setCursor(0,0);
	GFXfont *oldFont = lcd.getFont();
	lcd.setFont(&Picopixel);
	lcd.setTextColor(ST7735_GREEN);
	lcd.println("1157nm Clock Laser Ram Controller");
	lcd.setTextColor(ST7735_WHITE);
	lcd.println("Press any key for menu.");
	lcd.setFont(oldFont);
}

void promptXlims(){
	int xmin, xmax;
	
	xmin = numInput("Input x min: ");
	xmax = numInput("Input x max: ");
	
	plt.setXlims(xmin,xmax);	
}

void promptYlims(){
	int ymin, ymax;
	
	ymin = numInput("Input y min: ");
	ymax = numInput("Input y max: ");
	
	plt.setYlims(ymin,ymax);

}

