#ifndef temperatureController_h
#define temperatureController_h

#include "lcd.h"

namespace temperatureController {
	class thermistor;
	class tec;

	class PIcontroller;

	class PIcontroller{
		public:
			float G;     	//total gain
			float P, I;  	//p,i,d gains
			float output;	//output value
			float input;
			float setpoint;
			float error;
			float integral;
			float dt;

			void init(){G = P = I = output = input = setpoint = error = integral = dt = 0;};
			void feedback();	//call this loop repeatedly
	};

	class tec{
	public:
		unsigned int pin;
		float VCC;
		int resolution;

		tec(unsigned int pinn,float voltage,int res){analogWriteResolution(res); pin = pinn; VCC = voltage; resolution = res;};
		~tec(){};

		void setVoltage(float V){
			analogWriteResolution(resolution);
			float integer = ((1 << resolution) - 1 )*V/VCC;
			if ( (V >= 0)){
				int set = min(int(round(integer)), 1 << resolution);
				analogWrite(pin, set);
			} else {
				analogWrite(pin, 0);
			}
		}
	};

	class thermistor{
		public:
			unsigned int pin;
			float VCC;
			float resistance; //ohms
			float roomResistance;
			float readRate; //number per second
			float intVal;
			int resolution;
			
		public:

			thermistor(unsigned int analogPin, unsigned int ADC_Resolution);

			
			//definer functions
			void setVoltage(float voltage);   //set voltage driving voltage divider.
			void setResistorDivider(float R); //set value of top resistor for the voltage divider;
			void setReadRate(float rate);     //set rate of desired update times in number per second;
			void setThermistorValue(float R){roomResistance = R;};
			
			//utilities
			void update();
			float getVoltage();
			float getAverageVoltage(int averages, int integrationTime);
			float getResistance();
			float getAverageResistance(int averages, int integrationTime);
			float getAverageTemperature(int averages, int integrationTime);
		 
	};
}

#endif
