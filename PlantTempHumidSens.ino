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
#include "menu.h"
#include "SD.h"
#include "Adafruit_Si7021.h"
#include "RTClib.h"
 
#define DEBUG

//utilities
/*
 * Header files for graphing. bitArray is a custom boolean array class.
 */
#include "bitArray.h"
#include "graph.h"

//applications
#include <math.h>      	
#include "MemoryFree.h"	//a class for checking ram.

/***** Global Variables ********/
	//global variables
		#define DATAPOINTS 24*60	//the number of datpoints we hold for the graph.
		char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
		float times[DATAPOINTS] = {0};

		Adafruit_Si7021 sensor = Adafruit_Si7021();
		RTC_PCF8523 rtc;

	//vars for holding data to plot for the last seven days.
		float lastHumidity[7][DATAPOINTS] = {0};
		float lastTemperature[7][DATAPOINTS] = {0};


/******* Function Headers ******/
void printHeader();

/******* begin main ************/
/******* begin main ************/
/******* begin main ************/		

void setup() {
	lcd_initialize();
	//set LCD brightness
	pinMode(12,OUTPUT);
	analogWrite(12,127);

	//debugging code
	#ifdef DEBUG
		Serial.begin(9600);
		lcd.print("DEBUG MODE:: Waiting for Serial");
		while(!Serial){
			lcd.print(".");
			delay(250);
		}
	#endif

	/******* Load SD Card and print connection nfo! ************/
	if(SD.begin(SD_CS)){
		lcd.println("SD card connected!");
	} else {
		lcd.println("SD card failed...");
	}

	if (!sensor.begin()) {
		lcd.println("Did not find Si7021 sensor!");
		while (true)
			;
	} else {
		lcd.println("Found Si7021 sensor...");
		lcd.print("Temperture: ");
		lcd.print(sensor.readTemperature());
		lcd.println();
		lcd.print("Humidity:");
		lcd.print(sensor.readHumidity());
		lcd.println();
	}

	if (! rtc.begin()) {
		lcd.println("Couldn't find RTC");
		while (1);
	} else {
		lcd.println("Found RTC...");
	}

	if (! rtc.initialized()) {
		lcd.println("RTC was NOT running!");
	} else {
		lcd.println("RTC was telling time...");
		lcd.println("Time read is: ");
		DateTime now = rtc.now();
		lcd.print(now.month(), DEC);
		lcd.print("/");
		lcd.print(now.day(), DEC);
		lcd.print("/");
		lcd.print(now.year(),DEC);
		lcd.print("  ");
		lcd.print(now.hour(),DEC);
		lcd.print(":");
		lcd.print(now.minute(),DEC);
		lcd.println();
	}

	for (int i = 0; i < DATAPOINTS; ++i)
	{	
	 	for (int j = 0; j < 7; ++j)
	 	{
	 		lastHumidity[j][i] = 0;
	 		lastTemperature[j][i] = 0;

			if(i == DATAPOINTS-1){
				lastHumidity[j][i] = 100;
				lastTemperature[j][i] = 100;
			}

		}
		times[i] = 10.*i;
	}

	delay(5000);
}

void loop() {
	cls();	//clear the screen
	printHeader();
	
	//define graph objects, and their plotting area
		graph humidityPlot(0,3*CHARH,lcd.width(),lcd.height());
		int colors[8] = {ST7735_BLUE,ST7735_RED, ST7735_ORANGE, ST7735_YELLOW,ST7735_GREEN, ST7735_BLUE,0xC159,ST7735_WHITE}; //hex is purple
		//first color is grid
	//temperature plot settings

	//humidity plot settings
		humidityPlot.setXtics(5);
		humidityPlot.setYtics(3);
		humidityPlot.setXlims(0,1440);
		humidityPlot.setYlims(0,100);
		
		humidityPlot.makeAxes();
		humidityPlot.makeGrid();

	//declare variables for looping
		int h = 0;	//index for humidity
		int t = 0;	//index for temperature plotting

	while(true){
		delay(100);
		//read sensor measurements & log
		DateTime now = rtc.now();
			//remember daysOfTheWeek[now.dayOfTheWeek()]
			//and now.unixtime()

		float humid = sensor.readHumidity();
		float temp  = sensor.readTemperature();
		
		int minutesSinceMidnight = now.hour()*60 + now.minute();
		int everyTenMinutes = (minutesSinceMidnight/10);

		lastHumidity[now.dayOfTheWeek()][everyTenMinutes] = humid;
		lastTemperature[now.dayOfTheWeek()][everyTenMinutes] = temp;
		
		#ifdef DEBUG
			Serial.print("Humidity = "); Serial.println(lastHumidity[now.dayOfTheWeek()][everyTenMinutes]);
			Serial.print("everyTenMinutes = "); Serial.println(everyTenMinutes);
			Serial.print("Today's Day Number is "); Serial.println(now.dayOfTheWeek());
		#endif
		
		float buffer[DATAPOINTS] = {0};
		//plot humidity and temperature on one plot over the last three days
		for (int i = 0; i < 7; ++i)
		{	
		 	for (int j = 0; j < DATAPOINTS; ++j)
		 	{
		 		buffer[j] = lastHumidity[i][j];
		 		times[j] = 10*j;
		 	}
		 	humidityPlot.plotData(i,times, buffer, DATAPOINTS);
		}
		humidityPlot.setColors(colors, 8);
		humidityPlot.drawGraph();
		//save data to externalfile
	}
}


/******* Helper Functions  ************/
	void printHeader(){
		/*
		 * Print pretty info so people know what this box is for.
		 * And some key press info.
		 */
		lcd.setCursor(0,0);
		GFXfont *oldFont = lcd.getFont();
		lcd.setFont(&Picopixel);
		lcd.setTextColor(ST7735_GREEN);
		lcd.println("   Plant Temperature and Humidity Sensor");
		lcd.setTextColor(ST7735_WHITE);
		lcd.setFont(oldFont);
	}
