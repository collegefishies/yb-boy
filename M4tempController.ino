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

float feedbackTime = 1;
float EOMtemps[DATAPOINTS] = {0};
float FiberErrors[DATAPOINTS] = {0};
float times[DATAPOINTS] = {0};
float outputs[DATAPOINTS] = {0};

RTC_Millis rtc;

temperatureController::TemperatureController ram;

void printTime();
void printHeader();

void setG(){
	ram.lockbox.G = numInput("Input G: ");
	ram.saveConfig(RAMBAK);
}

void setP(){
	ram.lockbox.P = numInput("Input P: ");
	ram.saveConfig(RAMBAK);
}

void setI(){
	ram.lockbox.I = numInput("Input I: ");
	ram.saveConfig(RAMBAK);
}

void switchFeedback(){
	ram.lockbox.G *= -1;
	ram.saveConfig(RAMBAK);
}

void setSetpoint(){
	ram.lockbox.setpoint = numInput("Input setpoint (C): ");
	ram.saveConfig(RAMBAK);
}

void setOutputOffset(){
	ram.lockbox.outputOffset = numInput("Input OutputOffset: ");
	ram.saveConfig(RAMBAK);
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
	"Zero the integrator" 
	};

void (*settingsProgs[SETPROGS])()= {
	printSettings,
	setSetpoint, 
	switchFeedback, 
	setG, 
	setP, 
	setI, 
	setOutputOffset,
	zeroIntegrator
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

	if(false == ram.loadConfig(RAMBAK)){
		lcd.println("Failed to load config.");
	} else {
		lcd.println("Loaded config! Success!");
	}
	wait();
}

unsigned int oldtime;

DateTime oldPrintTime;

void loop() {
	graph plt(4*CHARW,3*CHARH,lcd.width(),lcd.height()/2);
	graph tecPlt(4*CHARW,2*CHARH + lcd.height()/2, lcd.width(), lcd.height());
		
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

	float avgTemp;
	unsigned time;
	while(true){
		
		//print time every second
		if ( (rtc.now() - oldPrintTime).seconds() >= 1){
			// eraseTime(); printTime();  oldPrintTime = rtc.now();
		} 

		if(millis() - oldtime > feedbackTime*1000.){
			EOMtemps[i] = ram.lock();
			times[i] = millis()/1000.;
			outputs[i] = ram.lockbox.output;

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