/*  M4tempController.ino
 *  ===============================
 *  
 */


//low level software
#include "lcd.h"    //includes lcd class for screen communication
#include "input.h"  //includes keypad inputs
#include "menu.h"

//utilities
#include "bitArray.h"
#include "graph.h"

//applications
#include "temperatureController.h"
#include <RTClib.h>
#include <math.h>
#include "MemoryFree.h"

#include "ramSettings.h"	//header file to wrap ramSettings menu.
#include "eomSettings.h"	//need to put into it's own class eventually...


/***** Global Variables ********/
	//global variables
		#define LOGICPIN      	//pin where we read in whether or not we lock
		#define PROGS 6       	//number of programs in the settings menu
		#define DATAPOINTS 500	//the number of datpoints we hold for the graph.
		#define SWITCHPIN  A5

	//vars for holding data to plot
		float EOMtemps[DATAPOINTS] = {0};
		float FiberErrors[DATAPOINTS] = {0};
		float times[DATAPOINTS] = {0};
		float outputs[DATAPOINTS] = {0};

	//temperature controllers and time keeper objects
		RTC_Millis rtc;
		temperatureController::GeneralController ram;
		temperatureController::TemperatureController eom;

/******** Settings Menu Things ***************/

	//menu object for the settings menu
		menu settings;

	//helper functions for cleaner code
		void printTime();
		void printHeader();
		void printFreeRam(){
			lcd.println("Free RAM is: ");
			lcd.println(freeMemory());
			wait();
		}

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

	settings.loop = false;                        	//define settings to quit once we call a function
	                                              	//this is so we return automatically to the graph
	settings.defineTitle("---menu---");           	//title
	settings.defineMenuItems(settingsItems,PROGS);	//define items
	settings.defineMenuProgs(settingsProgs,PROGS);	// "        "

	
	eomSettings.loop = false;	//same as for settings
	eomSettings.defineTitle("-Temperature Lock Sett's-");
	eomSettings.defineMenuItems(eomMenu::settingsItems,EOMPROGS);
	eomSettings.defineMenuProgs(eomMenu::settingsProgs,EOMPROGS);

	ramSettings.loop = false;	//same as for settings
	ramSettings.defineTitle("-Temperature Lock Sett's-");
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

	/******* Load SD Card and print Info! ************/


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


DateTime oldPrintTime;

void loop() {
	cls();	//clear the screen
	printHeader();
	printMode(true);
	
	//define graph objects, and their plotting area
		graph plt(0,3*CHARH,lcd.width(),2*CHARH + lcd.height()/2);
		graph tecPlt(0,2*CHARH + lcd.height()/2, lcd.width(), lcd.height());
		
		
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


	//declare variables for looping application
		int i = 0;
		unsigned long lastPlotTime = millis();
		unsigned long oldtime = millis();
		float avgTemp = NAN;
		unsigned time;

		

	while(true){
		printMode(false);
		//either lock the eom or ram depending on what we want.
		if( eom.lockbox.locked && digitalRead(SWITCHPIN)){
			avgTemp = eom.lock();	//eom.lock() measures for a long time then returns the average
			                     	//temperature	 

		} else if (ram.lockbox.locked && digitalRead(SWITCHPIN)) {
			ram.lock();	
			avgTemp = eom.thermistor.getAverageTemperature(eom.averageNumber,3000); //average for three seconds.
		} else if (!digitalRead(SWITCHPIN)){
			avgTemp = eom.thermistor.getAverageTemperature(eom.averageNumber,3000);
		}



		//only log temperature if we measured it.
			if(!isnan(avgTemp)){
				EOMtemps[i] = avgTemp;
				times[i] = millis()/1000.;	//this will wrap around in 50? days?
				outputs[i] = eom.lockbox.output;
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


					// dataFile = SD.open("eomtemp.txt", O_READ);
					// if(dataFile){
					//	Serial.println("Reading File")
					//	while (dataFile.available()) {
					//		 Serial.write(dataFile.read());
					//	}
					//	dataFile.close();
					// } else {
					//	Serial.println("Error reading eomtemp.txt");
					// }
					

				avgTemp = NAN;
			}

		//update plot every second only.
			if(millis() - lastPlotTime > 1000){
				lastPlotTime = millis();
				tecPlt.plotData(0, times, outputs, DATAPOINTS);
				plt.plotData(0, times, EOMtemps, DATAPOINTS);
				plt.drawGraph();
				tecPlt.drawGraph();
			}
			
		//access settings menu if key is pressed
			char key = keypad.getKey();
			if(key){
				if(key == '*'){
					return;
				}
				lcd.setFont();
				settings.ui();
				oldtime = millis(); // don't integrate for the huge time we were in menu.
				cls();
			
				printHeader();
				printMode(true);
			}
	}
	
}


/******* Helper Functions  ************/
	void printMode(bool hard){
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
		lcd.setCursor(0,0);
		GFXfont *oldFont = lcd.getFont();
		lcd.setFont(&Picopixel);
		lcd.setTextColor(ST7735_GREEN);
		lcd.println("1157nm RAM Controller");
		lcd.setTextColor(ST7735_WHITE);
		lcd.println("Any=MENU *=RST");
		lcd.setFont(oldFont);
	}
