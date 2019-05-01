#ifndef temperatureController_cpp
#define temperatureController_cpp

#include "temperatureController.h"
#include <Arduino.h>
#include <math.h>

namespace temperatureController{
/*
 *  Temperature Controller
 */

TemperatureController::TemperatureController(){
	lockbox.init(); tec.init(); thermistor.init(); 
	averageNumber = 100; lastLockTime = millis(); feedbackTime = 0;
}

TemperatureController::TemperatureController(String name){
	backup = name;

}

float TemperatureController::lock(){
	float averageTemperature;
	if((millis() - lastLockTime)/1000. >= feedbackTime){
		lockbox.dt = (millis() - lastLockTime)/1000.;
		lastLockTime = millis();

		//measure
		averageTemperature = thermistor.getAverageTemperature(averageNumber,feedbackTime*1000);

		//feed to PI
		lockbox.input = averageTemperature;

		//feedback
		lockbox.feedback();
		tec.setVoltage(lockbox.output);

		return averageTemperature;
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
			lcd.println("No backup directory!");
			return false;
		}
	#else
		if(!SD.begin(SD_CS)){
			lcd.println("SD failed to initialize!");
			return false;
		}

		if(!SD.exists(dirName)){
			lcd.println("No backup directory!");
			return false;
		}

		File iofile = SD.open(fullFname, FILE_WRITE);
	#endif

		StaticJsonDocument<512> doc;
		DeserializationError error = deserializeJson(doc,iofile);
		if(error){
			lcd.println("Failed to read file, using default configuration.");
			return false;
		}

		lockbox.G           	= doc["G"];           
		lockbox.I           	= doc["I"];           
		lockbox.P           	= doc["P"];           
		lockbox.outputOffset	= doc["outputOffset"];
		lockbox.setpoint    	= doc["setpoint"];    
		lockbox.integral    	= doc["integral"];    

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

		File iofile = SD.open(fullFname, FILE_WRITE);
	#endif


		StaticJsonDocument<512> doc;

		doc["G"]           	= lockbox.G;
		doc["I"]           	= lockbox.I;
		doc["P"]           	= lockbox.P;
		doc["outputOffset"]	= lockbox.outputOffset;
		doc["setpoint"]    	= lockbox.setpoint;
		doc["integral"]    	= lockbox.integral;

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
		integral += dt*error;
		output = -G*(P*error + I*integral) + outputOffset; //note the negative feedback!
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
		float averagedVoltage = 0;
		for(int i = 0; i < averages; i++){
			averagedVoltage += getVoltage();
			delay(integrationTime/averages);
		}
		averagedVoltage = averagedVoltage/averages;
		return averagedVoltage;
	}
	
	float Thermistor::getResistance(){
		float measuredResistance;
		float measuredVoltage = getVoltage();
		measuredResistance = resistance*measuredVoltage /(VCC - measuredVoltage);

		if (measuredResistance < 100*resistance){
			return measuredResistance;
		} else {
			return -1;
		}
	};

	float Thermistor::getAverageResistance(int averages, int integrationTime){
		float measuredResistance;
		float measuredVoltage = getAverageVoltage(averages,integrationTime);
		measuredResistance = resistance*measuredVoltage /(VCC - measuredVoltage);

		if (measuredResistance < 100*resistance){
			return measuredResistance;
		} else {
			return -1;
		}

};
	
	float Thermistor::getAverageTemperature(int averages, int waitTime){
			 float averageResistance = getAverageResistance(averages,waitTime);
			 float RtR = averageResistance / roomResistance;
			 const float T0 = 273.15;

			 if (averageResistance > 100*resistance){
				return -1;
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
}

#endif
