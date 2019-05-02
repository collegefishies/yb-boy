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

#define SETPROGS 8
#define DATAPOINTS 500
#define RAMBAK "pdheom"

float EOMtemps[DATAPOINTS] = {0};
float FiberErrors[DATAPOINTS] = {0};
float times[DATAPOINTS] = {0};
float outputs[DATAPOINTS] = {0};

RTC_Millis rtc;

temperatureController::TemperatureController ram;

void printTime();
void printHeader();

void setRamVar(float& x, String prompt){
	char buff[50];
	sprintf(buff, "Current value is: %e", x);
	lcd.println(buff);
	String temp = numInput(prompt);
	if(temp.length() == 0){
		lcd.println("No change made.");
		delay(500);
		return;
	}

	x = temp.toFloat();
	ram.saveConfig(RAMBAK);
}

void setVar(float& x, String prompt){
	char buff[50];
	sprintf(buff, "Current value is: %e", x);
	lcd.println(buff);
	String temp = numInput(prompt);
	if(temp.length() == 0){
		lcd.println("No change made.");
		delay(500);
		return;
	}

	x = temp.toFloat();
}

void setFeedbackTime(){setRamVar(ram.feedbackTime, "Input feedback time:\n");}
void setG(){setRamVar(ram.lockbox.G, "Input G:\n");}
void setP(){setRamVar(ram.lockbox.P, "Input P:\n");}
void setI(){setRamVar(ram.lockbox.I, "Input I (1/s):\n");}
void setSetpoint(){setRamVar(ram.lockbox.setpoint, "Input setpoint (C):\n");};
void setOutputOffset(){setRamVar(ram.lockbox.outputOffset, "Input outputOffset:\n");}

void switchFeedback(){
	ram.lockbox.G *= -1;
	ram.saveConfig(RAMBAK);
}


void toggleFeedback(){
	ram.lockbox.locked = !ram.lockbox.locked;
}

void zeroIntegrator(){
	ram.lockbox.integral = 0;
	ram.saveConfig(RAMBAK);
}

void printSettings(){
	char buff[200];
	sprintf(buff, 
		"Total Gain: %.3e\nProportional: %.3f\nIntegral Gain: %.3e\nOutput Offset: %.3f\nSetpoint (C): %.3f\n",
		ram.lockbox.G, ram.lockbox.P,
		ram.lockbox.I, ram.lockbox.outputOffset,
		ram.lockbox.setpoint);
	lcd.println(buff);
	wait();
}

menu settings;
String settingsItems[SETPROGS]   = {
	"Print settings",
	"Set setpoint",
	"Switch feedback sign",
	"Set the total gain G", 
	"Set proportional gain P", 
	"Set integral gain I", 
	"Set output offset",
	"Zero the integrator",
	"Set feedbackTime" 
	};

void (*settingsProgs[SETPROGS])()= {
	printSettings,
	setSetpoint, 
	switchFeedback, 
	setG, 
	setP, 
	setI, 
	setOutputOffset,
	zeroIntegrator,
	setFeedbackTime
};

void setup() {
	lcd_initialize();

	rtc.begin(DateTime(2019,1,1,12,0,0));
	
	settings.loop = false;
	settings.defineTitle("-------PI BOX------");
	settings.defineMenuItems(settingsItems,SETPROGS);
	settings.defineMenuProgs(settingsProgs,SETPROGS);

	ram.setVoltage(3.3); 
	ram.setResolution(12);

	ram.thermistor.setResistorDivider(2.49e3);
	ram.thermistor.setThermistorValue(10e3);

	ram.thermistor.setAnalogPin(A3);
	ram.tec.setAnalogPin(A1);

	//set pins 
	if(SD.begin(SD_CS)){
		lcd.println("SD card connected!");
	} else {
		lcd.println("SD card failed.");
	}
	if(false == ram.loadConfig(RAMBAK)){
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

		avgTemp = ram.lock();

		if(!isnan(avgTemp)){
			EOMtemps[i] = avgTemp;
			times[i] = millis()/1000.;
			outputs[i] = ram.lockbox.output;
			i = (i + 1) % DATAPOINTS;
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
	lcd.println("1157nm Clock Laser Ram Controller");
	lcd.setTextColor(ST7735_WHITE);
	lcd.println("Press any key for menu.");
	lcd.setFont(oldFont);
}
