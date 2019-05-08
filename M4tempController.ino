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
#include "MemoryFree.h"
#include "ramSettings.h"
#include "eomSettings.h"

#define PROGS 4
#define DATAPOINTS 500

float EOMtemps[DATAPOINTS] = {0};
float FiberErrors[DATAPOINTS] = {0};
float times[DATAPOINTS] = {0};
float outputs[DATAPOINTS] = {0};

RTC_Millis rtc;
temperatureController::GeneralController ram;
temperatureController::TemperatureController eom;

void printTime();
void printHeader();
void printFreeRam(){
	lcd.println("Free RAM is: ");
	lcd.println(freeMemory());
	wait();
}


menu settings;

void printEOMsettings(){eomSettings.ui();}
void printRamSettings(){ramSettings.ui();}
void activateRAMfeedback(){
	ram.lockbox.integral = 0;
	ram.lockbox.outputOffset = eom.lockbox.output; 
	eom.lockbox.locked = false; 
	ram.lockbox.locked = true;
}
void activateTempFeedback(){eom.lockbox.locked = true; ram.lockbox.locked = false;}

String settingsItems[PROGS] = {
	"Temperature Feedback Settings",
	"DC RAM Settings",
	"Switch to RAM feedback",
	"Switch to Temp. feedback"
};

void (*settingsProgs[PROGS])() = {
	printEOMsettings,
	printRamSettings,
	activateRAMfeedback,
	activateTempFeedback
};



void setup() {
	lcd_initialize();

	settings.loop = false;
	settings.defineTitle("---menu---");
	settings.defineMenuItems(settingsItems,PROGS);
	settings.defineMenuProgs(settingsProgs,PROGS);

	
	eomSettings.loop = false;
	eomSettings.defineTitle("-Temperature Lock Sett's-");
	eomSettings.defineMenuItems(eomMenu::settingsItems,EOMPROGS);
	eomSettings.defineMenuProgs(eomMenu::settingsProgs,EOMPROGS);

	ramSettings.loop = false;
	ramSettings.defineTitle("-Temperature Lock Sett's-");
	ramSettings.defineMenuItems(ramMenu::settingsItems,RAMPROGS);
	ramSettings.defineMenuProgs(ramMenu::settingsProgs,RAMPROGS);

	eom.setVoltage(3.3); 
	ram.setVoltage(3.3);
	eom.setResolution(12);
	ram.setResolution(12);

	eom.thermistor.setResistorDivider(2.49e3);
	eom.thermistor.setThermistorValue(10e3);

	ram.thermistor.setAnalogPin(A2);
	eom.thermistor.setAnalogPin(A3);
	ram.tec.setAnalogPin(A1);
	eom.tec.setAnalogPin(A1);


	//set pins 
	if(SD.begin(SD_CS)){
		lcd.println("SD card connected!");
	} else {
		lcd.println("SD card failed.");
	}
	if(false == eom.loadConfig(EOMBAK)){
		lcd.println("Failed to load config.");
	} else {
		lcd.println("Loaded config! Success!");
	}
	wait();
}


DateTime oldPrintTime;

void loop() {
	graph plt(0,3*CHARH,lcd.width(),2*CHARH + lcd.height()/2);
	graph tecPlt(0,2*CHARH + lcd.height()/2, lcd.width(), lcd.height());
		
	cls();

	printHeader();

	plt.setBoundary(0);
	plt.setYtics(3);
	plt.setXtics(3);
	plt.setXauto();
	plt.makeAxes();
	plt.makeGrid();
	tecPlt.setBoundary(10);
	tecPlt.setYtics(3);
	tecPlt.makeGrid();
	tecPlt.makeAxes();

	int i = 0;

	unsigned long lastPlotTime = millis();
	unsigned long oldtime = millis();

	float avgTemp;
	unsigned time;
	while(true){
		
		//print time every second
		if ( (rtc.now() - oldPrintTime).seconds() >= 1){
			// eraseTime(); printTime();  oldPrintTime = rtc.now();
		} 

		avgTemp = eom.lock();
		ram.lock();


		if(!isnan(avgTemp)){
			EOMtemps[i] = avgTemp;
			times[i] = millis()/1000.;
			outputs[i] = eom.lockbox.output;
			i = (i + 1) % DATAPOINTS;

			//log data to sd card
			String dataString = "";

			dataString += times[(i-1+DATAPOINTS)%DATAPOINTS];
			dataString += '\t';
			dataString += avgTemp;

			File dataFile = SD.open("eomtemp.txt", FILE_WRITE);

			  // if the file is available, write to it:
			if (dataFile) {
			    dataFile.println(dataString);
			    dataFile.close();
			}
		}

		//plot every second
		if(millis() - lastPlotTime > 1000){
			lastPlotTime = millis();
			tecPlt.plotData(0, times, outputs, DATAPOINTS);
			plt.plotData(0, times, EOMtemps, DATAPOINTS);
			plt.drawGraph();
			tecPlt.drawGraph();
		}
			
		//access settings menu
		char key = keypad.getKey();
		if(key){
			lcd.setFont();
			settings.ui();
			oldtime = millis(); // don't integrate for the huge time we were in menu.
			cls();
			printHeader();
		}
	}
}

void printHeader(){
	lcd.setCursor(0,0);
	GFXfont *oldFont = lcd.getFont();
	lcd.setFont(&Picopixel);
	lcd.setTextColor(ST7735_GREEN);
	lcd.println("1157nm Clock Laser RAM Controller");
	lcd.setTextColor(ST7735_WHITE);
	lcd.println("Press any key for menu.");
	lcd.setFont(oldFont);
}
