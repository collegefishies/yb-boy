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

#define SETPROGS 0

#define DATAPOINTS 500

float feedbackTime = 1;
float EOMtemps[DATAPOINTS] = {0};
float FiberErrors[DATAPOINTS] = {0};
float times[DATAPOINTS] = {0};
float outputs[DATAPOINTS] = {0};

RTC_Millis rtc;

temperatureController::TemperatureController ram;

void printTime();
void printHeader();

menu settings;
String settingsItems[SETPROGS]   = {};
void (*settingsProgs[SETPROGS])()= {};

void setup() {
	lcd_initialize();

	rtc.begin(DateTime(2019,1,1,12,0,0));
	
	settings.defineTitle("-------PI BOX------");
	settings.defineMenuItems(settingsItems,SETPROGS);
	settings.defineMenuProgs(settingsProgs,SETPROGS);

	ram.setVoltage(3.3); 
	ram.setResolution(12);

	ram.thermistor.setResistorDivider(2.49e3);
	ram.thermistor.setThermistorValue(10e3);

	if(false == ram.loadConfig("test")){
		lcd.println("Failed to load config.");
	}
	wait();
}

unsigned int oldtime;

DateTime oldPrintTime;

void loop() {
	graph plt(0,3*CHARH,lcd.width(),lcd.height()/2);
	graph tecPlt(0,2*CHARH + lcd.height()/2, lcd.width(), lcd.height());
		
	cls();

	printHeader();

	plt.setBoundary(0);
	plt.setYtics(3);
	plt.setXauto();
	plt.makeAxes();
	plt.makeGrid();
	tecPlt.setBoundary(10);
	tecPlt.setYtics(3);
	tecPlt.makeAxes();

	int i = 0;
	unsigned oldtime = millis();

	

	lcd.println(ram.lockbox.setpoint);

	ram.lockbox.setpoint = 3.141531987621938756982317659816;	
	ram.lockbox.G = 10.985712038571209385702918357;
	ram.lockbox.P = 1235.2140192837509128735039218;
	ram.lockbox.I = 0.0894750987140958123579801327;
	ram.lockbox.integral = 123098572.2315123095812735891;
	ram.lockbox.outputOffset = 1.23132131232;

	ram.saveConfig("test");

	float avgTemp;
	unsigned time;
	while(true){
		
		//print time every second
		if ( (rtc.now() - oldPrintTime).seconds() >= 1){
			// eraseTime(); printTime();  oldPrintTime = rtc.now();
		} 

		if(millis() - oldtime > feedbackTime*1000.){
			EOMtemps[i] = ram.lock();
			times[i] = millis();

			tecPlt.plotData(0, times, outputs, DATAPOINTS);
			plt.plotData(0, times, EOMtemps, DATAPOINTS);
			plt.drawGraph();
			tecPlt.drawGraph();
			
			i = (i + 1) % DATAPOINTS;
		}

		//access settings menu
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



