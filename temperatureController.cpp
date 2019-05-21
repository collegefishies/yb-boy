#ifndef temperatureController_cpp
#define temperatureController_cpp

#include "temperatureController.h"
#include <Arduino.h>
#include <math.h>

namespace temperatureController{
/*
 *	General Controller
 */

float GeneralController::lock(){
	//measure continously
	float tempVoltage = thermistor.getAverageVoltage(averageNumber,feedbackTime*1000);;

	//only update class variable iff the measurement is real.
	if(!isnan(tempVoltage)){
		averageReading = tempVoltage;
	}


	if((micros() - lastLockTime)/1.e6 >= feedbackTime){
		if(isnan(averageReading)){
			return NAN;
		}

		lockbox.dt = (micros() - lastLockTime)/1.e6;
		lastLockTime = micros();

		//feed to PI
		lockbox.input = averageReading;

		//feedback
		lockbox.feedback();
		tec.setVoltage(lockbox.output);

		return averageReading;
	} else {
		//return nans if feedback didn't occur.
		return NAN;
	}
}

/*
 *  Temperature Controller
 */

TemperatureController::TemperatureController(){
	lockbox.init(); tec.init(); thermistor.init(); 
	averageReading = NAN;
	averageNumber = 100; lastLockTime = micros(); feedbackTime = 0;
}

TemperatureController::TemperatureController(String name){
	backup = name;
}

float TemperatureController::lock(){
	//measure continously
	float tempTemperature = thermistor.getAverageTemperature(averageNumber,feedbackTime*1000);

	//only update class variable iff the measurement is real.
	if(!isnan(tempTemperature)){
		averageReading = tempTemperature;
	}

	if((micros() - lastLockTime)/1.e6 >= feedbackTime){
		if(isnan(averageReading)){
			//return nan as average isn't done yet.
			return NAN;
		}

		lockbox.dt = (micros() - lastLockTime)/1.e6;
		lastLockTime = micros();

		//feed to PI
		lockbox.input = averageReading;

		//feedback
		lockbox.feedback();
		tec.setVoltage(lockbox.output);

		return averageReading;
	} else {
		//return nans if feedback didn't occur.
		return NAN;
	}
}

bool TemperatureController::loadConfig(String fname){
	String dirName = String("/backups");
	String fullFname = dirName + String("/") + fname + String(".bak");
	
	#ifdef expressMem_h
		Adafruit_SPIFlash_FAT::File iofile;
		bool fsInitialized = false;
		bool flashInitialized = flash.begin(FLASH_TYPE);
		if (flashInitialized){
			fsInitialized = fatfs.begin();
			if(fsInitialized){
				if(fatfs.exists(fullFname)){
					iofile = fatfs.open(fullFname);	
					if(!iofile){
						lcd.println("Failed to open file.");
					}
				} else {
					lcd.println("No such file: " + fullFname);
				}
				
			} else {
				lcd.println("FatFS failed to initialize!");
				return false;
			}
		} else {
			lcd.println("Flash failed to initialize!");
			return false;
		}

		if(!fatfs.exists(dirName)){
			lcd.println("No backup directory!");
			return false;
		}
	#else
		File iofile;
		if(!SD.begin(SD_CS)){
			lcd.println("SD failed to initialize!");
			return false;
		}

		if(!SD.exists(dirName)){
			lcd.println("No backup directory!");
			return false;
		}

		if(SD.exists(fullFname)){
			iofile = SD.open(fullFname);
			if(!iofile){
				lcd.println("Failed to open file.");
			}
		} else {
			lcd.println("No such file: " + fullFname);
		}
		
	#endif


		StaticJsonDocument<2048> doc;
		DeserializationError error = deserializeJson(doc,iofile);
		if(error){
			lcd.println("Failed to read file, using default configuration.");
			return false;
		}


		lockbox.G           	= doc["G"]           ;
		lockbox.I           	= doc["I"]           ;
		lockbox.P           	= doc["P"]           ;
		lockbox.outputOffset	= doc["outputOffset"];
		lockbox.setpoint    	= doc["setpoint"]    ;
		lockbox.integral    	= doc["integral"]    ;
		feedbackTime        	= doc["feedbackTime"];

		iofile.close();
		return true;
}

bool TemperatureController::saveConfig(String fname){
	String dirName = String("/backups");
	String fullFname = dirName + String("/") + fname + String(".bak");
	
	#ifdef expressMem_h
		Adafruit_SPIFlash_FAT::File iofile;
		bool fsInitialized = false;
		bool flashInitialized = flash.begin(FLASH_TYPE);

		if(fatfs.exists(fullFname)){
			fatfs.remove(fullFname);
		}

		if (flashInitialized){
			fsInitialized = fatfs.begin();
			if(fsInitialized){
				iofile = fatfs.open(fullFname, FILE_WRITE);
			} else {
				lcd.println("FatFS failed to initialize!");
				return false;
			}
		} else {
			lcd.println("Flash failed to initialize!");
			return false;
		}

		if(!fatfs.exists(dirName)){
			if(!fatfs.mkdir(dirName)){
				lcd.println("Failed to make directory");
				return false;
			}
		}

		if(!iofile){
			lcd.println("Failed to open file for writing.");
		}
	#else
		if(!SD.begin(SD_CS)){
			lcd.println("SD failed to initialize!");
			return false;
		}

		if(!SD.exists(dirName)){
			if(!SD.mkdir(dirName)){
				lcd.println("Failed to make directory");
				return false;
			}
		}

		if(SD.exists(fullFname)){
			SD.remove(fullFname);
		}

		File iofile = SD.open(fullFname, FILE_WRITE);

		if(!iofile){
			lcd.println("Failed to open file for writing.");
		}
	#endif


		StaticJsonDocument<2048> doc;

		doc["G"]           	= lockbox.G;
		doc["I"]           	= lockbox.I;
		doc["P"]           	= lockbox.P;
		doc["outputOffset"]	= lockbox.outputOffset;
		doc["setpoint"]    	= lockbox.setpoint;
		doc["integral"]    	= lockbox.integral;
		doc["feedbackTime"] = feedbackTime;

		if(serializeJson(doc,iofile) == 0){
			lcd.println("Failed to write file.");
			return false;
		}

		iofile.close();
		return true;

}
/*
 *  PI Controller
 */

	void PIcontroller::feedback(){
		error = input - setpoint;
		if(locked){
			integral += dt*error;
			output = -G*(P*error + I*integral) + outputOffset; //note the negative feedback!
		}
	}

	const int averageNumber = 100;
/*
 * Thermistor Functions
 */

	
//definer functions
	//set voltage driving voltage divider.
	void Thermistor::setVoltage(float voltage){   VCC = voltage;  };
	//set value of top resistor for the voltage divider;
	void Thermistor::setResistorDivider(float R){   resistance = R;   }; 
	//set rate of desired update times in number per second;
	// void Thermistor::setReadRate(float rate){   readRate = rate;    };     

//utility functions
	void Thermistor::update(){
		intVal = analogRead(pin);
	}

	float Thermistor::getVoltage(){
			float voltage;
			update();
			voltage = (VCC) * intVal / (pow(2,resolution) - 1);
			return voltage;
		}

	float Thermistor::getAverageVoltage(int averages, int integrationTime){
		//non blocking averager
		
		//loop using class variables.
		if(avgNo < averages && (millis()-lastMeasureTime) > (integrationTime/averages)){
			// Serial.print("AvgNo: ");
			// Serial.print(avgNo);
			// Serial.print(" RunningAvg = ");
			lastMeasureTime = millis();
			averagedVoltage += getVoltage();
			avgNo += 1;
			// Serial.println(averageReading);
		} else if (avgNo == averages){
			//reset class variables and spit out average

			float tempAverage = averagedVoltage/averages;
			averagedVoltage = 0;
			avgNo = 0;
			return tempAverage;
		}

		return NAN;
	}
	
	float Thermistor::getResistance(){
		float measuredResistance;
		float measuredVoltage = getVoltage();
		measuredResistance = resistance*measuredVoltage /(VCC - measuredVoltage);

		if (measuredResistance < 100*resistance){
			return measuredResistance;
		} else {
			return NAN;
		}
	};

	float Thermistor::getAverageResistance(int averages, int integrationTime){
		float measuredResistance;
		float measuredVoltage = getAverageVoltage(averages,integrationTime);
		
		//return nan if non blocking average isn't done.
		if(isnan(measuredVoltage)){
			return NAN;
		}

		measuredResistance = resistance*measuredVoltage /(VCC - measuredVoltage);

		if (measuredResistance < 100*resistance){
			return measuredResistance;
		} else {
			return NAN;
		}

};
	
	float Thermistor::getAverageTemperature(int averages, int waitTime){
			 float averageResistance = getAverageResistance(averages,waitTime);
			 
			 if(isnan(averageResistance)){
				//return nan if blocking average wasn't done.
				return NAN;
			 }

			 float RtR = averageResistance / roomResistance;
			 const float T0 = 273.15;

			 if (averageResistance > 100*resistance){
				return NAN;
			 }

			 float T;
			 
			 const float a = 3.3530481e-3;
			 const float b = 2.5420230e-4;
			 const float c = 1.1431163e-6;
			 const float d = -6.9383563e-8;

			 float logRtR = log(RtR);

			 T = 1/(a + b*logRtR+ c*pow(logRtR,2) + d*pow(logRtR,3));
			 return T-T0;
	};

void printFile(const char *filename) {
  // Open file for reading
  File file = SD.open(filename);
  if (!file) {
    Serial.println(F("Failed to read file"));
    return;
  }

  // Extract each characters by one by one
  while (file.available()) {
    Serial.print((char)file.read());
  }
  Serial.println();

  // Close the file
  file.close();
}

}

#endif
