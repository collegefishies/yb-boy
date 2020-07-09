/*  M4tempController.ino
 *  ===============================
 *	 A temperature controller built on the Feather M4 Express board.
 *   External interfacing is done mostly using A0-A5 pins, their usage
 *   is in this file.
 */


//low level software
/*
 * Edit these libraries if you have different pin outs!
 *
 */ 
#include "lcd.h"    //includes lcd class for screen communication
#include "input.h"  //includes keypad inputs
#include "menu.h"

//utilities
/*
 * Header files for graphing. bitArray is a custom boolean array class.
 */
#include "bitArray.h"
#include "graph.h"

//applications
#include "temperatureController.h"	//holds the object classes that do the controlling
//#include <RTClib.h>             	//a class for interfacing with external/internal real time clocks
#include <math.h>                 	
#include "MemoryFree.h"           	//a class for checking ram.

#include "ramSettings.h"	//header file to wrap ramSettings menu.
#include "eomSettings.h"	//need to put into it's own class eventually...


/***** Global Variables ********/
	//global variables
		#define PROGS 6       	//number of programs in the settings menu
		#define DATAPOINTS 500	//the number of datpoints we hold for the graph.
		#define SWITCHPIN  A5 	//pin used for switching between 

	//vars for holding data to plot
		//for graph of temperatures and drive values
		float EOMtemps[DATAPOINTS] = {0};
		float outputs[DATAPOINTS] = {0};
		float times[DATAPOINTS] = {0};

		//for graph of RAM values and times
		float RAMvoltages[DATAPOINTS] = {0};
		float RAMtimes[DATAPOINTS] = {0};

		//for graph of fiber errors (not yet used)
		float FiberErrors[DATAPOINTS] = {0};
		

	//temperature controllers and time keeper objects
		//RTC_Millis rtc;
		temperatureController::GeneralController ram;
		temperatureController::TemperatureController eom;

/******** Settings Menu Things ***************/

	//menu object for the settings menu
		menu settings;

	//helper functions for cleaner code
		void printTime();
		void printHeader();
		#ifdef MEMORY_FREE_H
		void printFreeRam(){
			lcd.println("Free RAM is: ");
			lcd.println(freeMemory());
			wait();
		}
		#endif

	//functions to be called in the settings menu.
		void printPinOuts(){
			lcd.print("eom sense pin: A"); lcd.println(eom.thermistor.pin-A0);
			lcd.print("ram sense pin: A"); lcd.println(ram.thermistor.pin-A0);
			lcd.print("eom drive pin: A"); lcd.println(eom.tec.pin-A0);
			lcd.print("ram drive pin: A"); lcd.println(ram.tec.pin-A0);
			lcd.print("switch pin:    A"); lcd.println(SWITCHPIN-A0);
			wait();
		}
		void printEOMsettings(){eomSettings.ui();}
		void printRamSettings(){ramSettings.ui();}
		void activateRAMfeedback(){
			ram.lockbox.integral = 0;
			ram.lockbox.outputOffset = eom.lockbox.output; 
			eom.lockbox.locked = false;
			ram.lockbox.locked = true;
		}
		void activateTempFeedback(){eom.lockbox.locked = true; ram.lockbox.locked = false;}
		void rotateScreenUI(){
			lcd.println("[1-4] Rotation Type, # to exit");
			while(true){
				
				char key = keypad.getKey();
				
				switch(key){
					case '1':
					case '2':
					case '3':
					case '4':
						lcd.setRotation(key - '1' + 1);
						cls();
						lcd.println("[1-4] Rotation Type, # to exit");
						break;
					case '#':
						return;
						break;
				}
			}
		}
	//names of the settings menu items
		String settingsItems[PROGS] = {
			"Temperature Feedback Settings",
			"DC RAM Settings",
			"Switch to RAM feedback",
			"Switch to Temp. feedback",
			"Print Pinouts",
			"Rotate Screen"
		};
	//program array for the settings menu
		void (*settingsProgs[PROGS])() = {
			printEOMsettings,
			printRamSettings,
			activateRAMfeedback,
			activateTempFeedback,
			printPinOuts,
			rotateScreenUI
		};


/******* begin main ************/
/******* begin main ************/
/******* begin main ************/		

void setup() {
	lcd_initialize();
	Serial.begin(9600);
	
	/******* Menu Initializations **********/

	settings.loop = false;	//define settings to quit once we call a function
	                      	//this is so we return automatically to the graph

	settings.defineTitle("---menu---");           	//title
	settings.defineMenuItems(settingsItems,PROGS);	//define items
	settings.defineMenuProgs(settingsProgs,PROGS);	// "        "

	
	eomSettings.loop = false;	//same as for settings
	eomSettings.defineTitle("-Temp. Lock Sett's-");
	eomSettings.defineMenuItems(eomMenu::settingsItems,EOMPROGS);
	eomSettings.defineMenuProgs(eomMenu::settingsProgs,EOMPROGS);

	ramSettings.loop = false;	//same as for settings
	ramSettings.defineTitle("-RAM Lock Settings-");
	ramSettings.defineMenuItems(ramMenu::settingsItems,RAMPROGS);
	ramSettings.defineMenuProgs(ramMenu::settingsProgs,RAMPROGS);


	/****** Temperature/RAM Controller Settings *********/

	eom.setVoltage(3.3);  	//define the board voltage to be 3.3V so it correctly outputs the right value.
	ram.setVoltage(3.3);  	//
	eom.setResolution(12);	//set the AnalogDAC and ADC resolution to be 12bits
	ram.setResolution(12);	//

	eom.thermistor.setResistorDivider(2.49e3);	//Set the value of the top resistor value in the voltage divider
	eom.thermistor.setThermistorValue(10e3);  	//set the 25C temperature of the thermistor for correct temperature
	                                          	//calculations

	ram.thermistor.setAnalogPin(A2);	//set the ADC pin for the ram.thermistor
	eom.thermistor.setAnalogPin(A3);	//set the ADC pin for the eom.thermistor
	ram.tec.setAnalogPin(A1);       	//set the DAC pin for the ram compensation
	eom.tec.setAnalogPin(A1);       	//set DAC for EOM, note it's the same pin! because both 
	                                	//feedback mechanisms are the same! they don't interfere.
	pinMode(SWITCHPIN, INPUT);

	/******* Load SD Card and print connection nfo! ************/
	if(SD.begin(SD_CS)){
		lcd.println("SD card connected!");
	} else {
		lcd.println("SD card failed.");
	}

	if(false == eom.loadConfig(EOMBAK)){            //check to see if we have settings backed up.
		lcd.println("Failed to load TEMP config.");
	} else {
		lcd.println("Loaded TEMP config!");
	}
	if(false == ram.loadConfig(RAMBAK)){            //check to see if we have settings backed up.
		lcd.println("Failed to load RAM config.");
	} else {
		lcd.println("Loaded RAM config!");
	}
	wait();	//begin

	activateTempFeedback();	//lock temperature just as a default.
}

void loop() {
	cls();	//clear the screen
	printHeader();
	printMode(true);	//print which lock we have.
	
	//define graph objects, and their plotting area
		graph plt(0,3*CHARH,lcd.width(),2*CHARH + lcd.height()/2);
		graph tecPlt(0,2*CHARH + lcd.height()/2, lcd.width()/2, lcd.height());
		graph ramPlt(lcd.width()/2,2*CHARH + lcd.height()/2, lcd.width(), lcd.height());
		
	//temperature plot settings 
		plt.setBoundary(0);
		plt.setYtics(3);
		plt.setXtics(3);
		plt.setXauto();
		plt.makeAxes();
		plt.makeGrid();
	//TEC output plot settings
		tecPlt.setBoundary(10);
		tecPlt.setYtics(3);
		tecPlt.makeGrid();
		tecPlt.makeAxes();
		tecPlt.setXtics(3);
	//RAM output plot settings
		ramPlt.setBoundary(10);
		ramPlt.setYtics(3);
		ramPlt.setXtics(3);
		ramPlt.makeGrid();
		ramPlt.makeAxes();


	//declare variables for looping application
		int i = 0;	//index for temperature/output plotting
		int j = 0;	//index for RAM plotting
		unsigned long lastPlotTime = millis();
		float avgTemp = NAN;   	//temperature measurements
		float ramVoltage = NAN;	//ram drive measurements

	//run time loop.
	while(true){
		printMode(false);	//only update label when lock mode has changed.

		/*
		 *  Switch between appropriate locking modes
		 *  and call the associated lock function.
		 */

		//either lock the eom or ram depending on what we want.
		if( eom.lockbox.locked && digitalRead(SWITCHPIN)){
			avgTemp = eom.lock();	//eom.lock() measures for a eom.feedbackTime then returns the average
			                     	//temperature
			
			ram.lastLockTime = eom.lastLockTime;	//set both locks to have equal last lock time so they don't
			                                    	//integrate massive (false) errors when switching between locks.
		} else if (ram.lockbox.locked && digitalRead(SWITCHPIN)) {
			ramVoltage = ram.lock();
			eom.lastLockTime = ram.lastLockTime;                            	//set both locks to have equal last lock time so they don't
			                                                                	//integrate massive (false) errors when switching between locks.
			eom.lockbox.history = - eom.lockbox.output + ram.lockbox.output;	//make outputs continous when you switch drives.

			avgTemp = eom.thermistor.getAverageTemperature(eom.averageNumber,3000); //average for three seconds.

			//switch to temperature locking if we've strayed too far. clearly ram lock isn't fedback properly
			if(abs(avgTemp - eom.lockbox.setpoint) > 1){
				activateTempFeedback();
			}


		} else if (!digitalRead(SWITCHPIN)){
			//just log temperature if something else is controlling our eom temperature.
			avgTemp = eom.thermistor.getAverageTemperature(eom.averageNumber,3000);

			ram.lastLockTime = eom.lastLockTime = micros();	//say we haven't locked it, while we've been logging.
		}



		//Log temperature to SD card and update graph data.
			if(!isnan(avgTemp)){	//only log temperature if we measured it.

				EOMtemps[i] = avgTemp;
				times[i] = millis()/1000.;	//this will wrap around in 50? days?

				if(eom.lockbox.locked && digitalRead(SWITCHPIN)){
					outputs[i] = eom.lockbox.output;
				} else if (ram.lockbox.locked && digitalRead(SWITCHPIN)){
					outputs[i] = ram.lockbox.output;
				}

				i = (i + 1) % DATAPOINTS;

				//log data to sd card
					String dataString = "";

					dataString += times[(i-1+DATAPOINTS)%DATAPOINTS];
					dataString += '\t';
					dataString += avgTemp;

					File dataFile = SD.open("eomtemp.txt", O_WRITE | O_APPEND | O_CREAT);
					// if the file is available, write to it:
						if (dataFile) {
							Serial.print("Writing: ");
							Serial.println(dataString);
						    dataFile.println(dataString);
						    dataFile.close();
						} else {
							Serial.println("Error writing to eomtemp.txt");
						}

				avgTemp = NAN;
			}

		//only log RAM if we measured it
		//update RAM graph and SD card values.
			if(!isnan(ramVoltage)){
				RAMvoltages[j] = ramVoltage;
				RAMtimes[j] = millis()/1000.;	//this will wrap around in 50? days?
				j = (j + 1) % DATAPOINTS;

				//log data to sd card
					String dataString = "";

					dataString += RAMtimes[(j-1+DATAPOINTS)%DATAPOINTS];
					dataString += '\t';
					dataString += String(ramVoltage,4);

					File dataFile = SD.open("ramvals.txt", O_WRITE | O_APPEND | O_CREAT);
					// if the file is available, write to it:
						if (dataFile) {
							Serial.print("Writing: ");
							Serial.println(dataString);
						    dataFile.println(dataString);
						    dataFile.close();
						} else {
							Serial.println("Error writing to ramvals.txt");
						}
						
				ramVoltage = NAN;
			}

		//update plot every second only.
			if(millis() - lastPlotTime > 1000){
				//remember when we plotted.
				lastPlotTime = millis();

				//plot the data to the first trace for each of the graphs.
				tecPlt.plotData(0, times, outputs, DATAPOINTS);
				ramPlt.plotData(0, RAMtimes, RAMvoltages, DATAPOINTS);
				plt.plotData(0, times, EOMtemps, DATAPOINTS);

				//don't forget to draw the graphs!
				plt.drawGraph();
				tecPlt.drawGraph();
				ramPlt.drawGraph();
			}
		


		//access settings menu if key is pressed
			char key = keypad.getKey();
			if(key){
				if(key == '*'){
					//kill loop() and then call loop()
					//for resizing the graphs if we change the screen rotation.
					return;
				}

				lcd.setFont();
				settings.ui();	//menu ui call.
				
				//reset display
				cls();
				printHeader();
				printMode(true);
			}
	}
	
}


/******* Helper Functions  ************/
	void printMode(bool hard){
		/*
		 * printLocking mode. If hard==true, always erase and print new label
		 If hard==false, only print new label if mode has changed.
		 */
		static int mode;
		if(hard){
			mode = -1;
		}

		lcd.setFont();

		int x0,y0;
		x0 = lcd.getCursorX(); y0 = lcd.getCursorY();

		
		if( eom.lockbox.locked && digitalRead(SWITCHPIN)){
			if(mode != 1 ){
				mode = 1;
				
				lcd.setCursor(lcd.width() - 3*CHARW, 0);
				lcd.fillRect(lcd.width()-3*CHARW,0,3*CHARW,CHARH, BACKGROUND);
				lcd.print("TMP");
			}
		} else if (ram.lockbox.locked && digitalRead(SWITCHPIN)) {
			if(mode != 2){
				mode = 2;
				
				lcd.setCursor(lcd.width() - 3*CHARW, 0);
				lcd.fillRect(lcd.width()-3*CHARW,0,3*CHARW,CHARH, BACKGROUND);
				lcd.print("RAM");
			}

		} else if (!digitalRead(SWITCHPIN)){
			if(mode != 3){
				mode = 3;
				
				lcd.setCursor(lcd.width() - 3*CHARW, 0);
				lcd.fillRect(lcd.width()-3*CHARW,0,3*CHARW,CHARH, BACKGROUND);
				lcd.print("LOG");
			}
		}
		lcd.setCursor(x0,y0);
	}


	void printHeader(){
		/*
		 * Print pretty info so people know what this box is for.
		 * And some key press info.
		 */
		lcd.setCursor(0,0);
		GFXfont *oldFont = lcd.getFont();
		lcd.setFont(&Picopixel);
		lcd.setTextColor(ST7735_GREEN);
		lcd.println("1157nm RAM Controller");
		lcd.setTextColor(ST7735_WHITE);
		lcd.println("Any=MENU *=RST");
		lcd.setFont(oldFont);
	}
