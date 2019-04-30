#ifndef temperatureController_h
#define temperatureController_h

namespace temperatureController {
	class thermistor;

	class PIcontroller;

	class PIcontroller{
		public:
			float G;     	//total gain
			float P, I;  	//p,i,d gains
			float output;	//output value
			float input;
			float setPoint;
			float error;
			float integral;
			float dt;

			void feedback();	//call this loop repeatedly
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
			float getAverageVoltage(int averages, int waitTime);
			float getResistance();
			float getAverageResistance(int averages, int waitTime);
			float getAverageTemperature(int averages, int waitTime);
		 
	};
}

#endif
