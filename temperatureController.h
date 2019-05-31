#ifndef temperatureController_h
#define temperatureController_h

#include "lcd.h"
#include <ArduinoJson.h>
#include <SD.h>
#include "expressMem.h"

namespace temperatureController {

	class ControllerInterface;
	class TemperatureController;
	class Thermistor;
	class TEC;
	class PIcontroller;

	class ControllerWithInterface : public GeneralController;  
	class TemperatureControllerWithInterface: public TemperatureController;

	class PIcontroller{
		public:
			float G;     	//total gain
			float P, I;  	//p,i,d gains
			float output;	//output value
			float outputOffset;
			float input;
			float setpoint;
			float error;
			float integral;
			float dt;

			bool locked;
			
			void init(){G = P = I = output = input = setpoint = error = integral = dt = locked = 0;};
			void feedback();	//call this loop repeatedly
	};

	class TEC{
	public:
		unsigned int pin;
		float VCC;
		int resolution;

		void init(){pin = VCC = resolution = 0;}
		void setResolution(unsigned int res){resolution = res; analogWriteResolution(resolution);}
		void setMaxVoltage(float V){VCC = V;}
		void setAnalogPin(unsigned int analogPin){pin = analogPin;}
		void setVoltage(float V){
			analogWriteResolution(resolution);
			float integer = ((1 << resolution) - 1 )*V/VCC;
			if ( (V >= 0)){
				int set = min(int(round(integer)), (1 << resolution) - 1);
				analogWrite(pin, set);
			} else {
				analogWrite(pin, 0);
			}
		}
	};

	class Thermistor{
		public:
			unsigned int pin;
			float VCC;
			float resistance; //ohms
			float roomResistance;
			float intVal;
			int resolution;
			float averagedVoltage;
			unsigned int lastMeasureTime;
			unsigned int avgNo;

		public:

			//definer functions
			void init(){pin = VCC = resistance = roomResistance = intVal = resolution = lastMeasureTime = avgNo = averagedVoltage = 0;}
			void setResolution(unsigned int res){resolution = res; analogReadResolution(resolution);}
			void setAnalogPin(unsigned int analogPin){pin = analogPin; update();}
			void setVoltage(float voltage);  	//set voltage driving voltage divider.
			void setResistorDivider(float R);	//set value of top resistor for the voltage divider;
			// void setReadRate(float rate); 	//set rate of desired update times in number per second;
			void setThermistorValue(float R){roomResistance = R;};
			
			//utilities
			void update();                                                 	//updates intVal with reading of thermistor;
			float getVoltage();                                            	//
			float getAverageVoltage(int averages, int integrationTime);    	//
			float getResistance();                                         	//
			float getAverageResistance(int averages, int integrationTime); 	//
			float getAverageTemperature(int averages, int integrationTime);	//
		 
	};

	class TemperatureController{
	public:
		PIcontroller lockbox;
		Thermistor thermistor;
		TEC tec;
		String backup;

		unsigned long lastLockTime;
		float feedbackTime;
		float averageReading;
		unsigned int averageNumber;
 
		TemperatureController();
		TemperatureController(String name); //pass backup name 
		~TemperatureController(){}

		//initializers
		// void setBackupFilename(String name){backup = name;}
		void setVoltage(float V){tec.setMaxVoltage(V); thermistor.setVoltage(V);}
		void setResolution(unsigned int res){tec.setResolution(res); thermistor.setResolution(res);}

		//lock loops
		float lock();

		//ui functions
		void determineOutputOffset(){
			lcd.setFont();
			lcd.println("Determine Output Offset via increasing it until ");
		}

		bool saveConfig(String fname);
		bool loadConfig(String fname);
	};

	class GeneralController : public TemperatureController{
	public:
		float lock();
	};

	class ControllerWithInterface : public GeneralController{
	public:
		menu settings;

		void initSettings(String title){
			settings.loop = false;	//same as for settings
			settings.defineTitle(title);
			settings.defineMenuItems(ramMenu::settingsItems,RAMPROGS);
			settings.defineMenuProgs(ramMenu::settingsProgs,RAMPROGS);
		}

		void setSaveVar(float& x, String prompt);
		void setVar(float& x, String prompt)

		void setFeedbackTime(){setSaveVar(feedbackTime, "Input feedback time:\n");}
		void setG(){setSaveVar(lockbox.G, "Input G:\n");}
		void setP(){setSaveVar(lockbox.P, "Input P:\n");}
		void setI(){setSaveVar(lockbox.I, "Input I (1/s):\n"); lockbox.integral = 0;}
		void setSetpoint(){setSaveVar(lockbox.setpoint, "Input setpoint (V):\n");};
		void setOutputOffset(){setSaveVar(lockbox.outputOffset, "Input outputOffset:\n");}
		void switchFeedback(){lockbox.G *= -1;saveConfig(RAMBAK);}
		void toggleFeedback(){lockbox.locked = !lockbox.locked;}
		void zeroIntegrator(){lockbox.integral = 0;saveConfig(RAMBAK);}
		void printSettings();
		void copyOutputToRAMfromTEMP(){lockbox.outputOffset = eom.lockbox.output;}

		const String settingsItems[RAMPROGS]   = {
			"Set Temp. Out to RAM OutOffs.",
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

		const void (*settingsProgs[RAMPROGS])()= {
			copyOutputToRAMfromTEMP,
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

	};

	class TemperatureControllerWithInterface : public TemperatureController{
	public:
		void setSaveVar(float& x, String prompt){
			char buff[50];
			sprintf(buff, "Current value is:\n%e", x);
			lcd.println(buff);
			String temp = numInput(prompt);
			if(temp.length() == 0){
				lcd.println("No change made.");
				delay(500);
				return;
			}

			x = temp.toFloat();
			saveConfig(EOMBAK);
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

		void setFeedbackTime(){setSaveVar(feedbackTime, "Input feedback time:\n");}
		void setG(){setSaveVar(lockbox.G, "Input G:\n");}
		void setP(){setSaveVar(lockbox.P, "Input P:\n");}
		void setI(){setSaveVar(lockbox.I, "Input I (1/s):\n"); lockbox.integral = 0;}
		void setSetpoint(){setSaveVar(lockbox.setpoint, "Input setpoint (C):\n");};
		void setOutputOffset(){setSaveVar(lockbox.outputOffset, "Input outputOffset:\n");}
		void switchFeedback(){lockbox.G *= -1;saveConfig(EOMBAK);}
		void toggleFeedback(){lockbox.locked = !lockbox.locked;}
		void zeroIntegrator(){lockbox.integral = 0;saveConfig(EOMBAK);}
		void printFreeRam(){lcd.println("Free RAM is: ");lcd.println(freeMemory());wait();}
		void printSettings(){
			char buff[200];
			sprintf(buff, 
				"Total Gain: %.3e\nProportional: %.3f\nIntegral Gain: %.3e\nOutput Offset: %.3f\nSetpoint (C): %.3f\n",
				lockbox.G, lockbox.P,
				lockbox.I, lockbox.outputOffset,
				lockbox.setpoint);
			lcd.println(buff);
			wait();
		}

		const String settingsItems[EOMPROGS]   = {
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

		const void (*settingsProgs[EOMPROGS])()= {
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

	}

}

#endif
