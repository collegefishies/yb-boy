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

#define DATAPOINTS 100

float feedbackTime = 1;
float EOMtemps[DATAPOINTS] = {0};
float FiberErrors[DATAPOINTS] = {0};
float times[DATAPOINTS] = {0};
float outputs[DATAPOINTS] = {0};

RTC_Millis rtc;
temperatureController::thermistor EOM(A2,12);
temperatureController::PIcontroller eomPI; 

void printTime();
void printHeader();

menu settings;
String settingsItems[SETPROGS]   = {};
void (*settingsProgs[SETPROGS])()= {};

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

unsigned int oldtime;


void loop() {
	graph plt(0,3*CHARH,lcd.width(),lcd.height());
		
	cls();
	printHeader();
	plt.setBoundary(10);
	plt.setYlims(20,70);
	plt.setXauto();
	plt.makeAxes();
	plt.makeGrid();

	eomPI.G       	= 1;
	eomPI.P       	= 1;
	eomPI.I       	= 1;
	eomPI.setpoint	= 50; 

	int i = 0;
	oldtime = millis();
	while(true){
		
		//print time every second
		if ( (rtc.now() - oldPrintTime).seconds() >= 1){
			eraseTime(); printTime();  oldPrintTime = rtc.now();
		} 


		if( (millis() - oldtime)/1000. >= feedbackTime){
			//measure
			EOMtemps[i] = EOM.getResistance();
			times[i] = millis()/1000.;

			//feed to PI
			eomPI.input = EOMtemps[i];

			//feedback
			eomPI.feedback();
			outputs[i] = eomPI.output;
			
			plt.plotData(0, times, EOMtemps, DATAPOINTS);
			plt.plotData(1, times, outputs, DATAPOINTS);
			plt.drawGraph();
			
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



