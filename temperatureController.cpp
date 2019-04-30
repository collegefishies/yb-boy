#ifndef temperatureController_cpp
#define temperatureController_cpp

#include <Arduino.h>
#include "temperatureController.h"
#include <math.h>

namespace temperatureController{

/*
 *  Temperature Controller
 */

  void PIcontroller::feedback(){
    error = input - setPoint;
    integral += dt*error;
    output = -G*(P*error + I*integral); //note the negative feedback!
  }

  const int averageNumber = 100;
/*
 * Thermistor Functions
 */

//constructors
  thermistor::thermistor(unsigned int analogPin, unsigned int ADC_Resolution){
    pin = analogPin;
    resolution = ADC_Resolution;
    analogReadResolution(resolution);
    update();
  }
  
//definer functions
  //set voltage driving voltage divider.
  void thermistor::setVoltage(float voltage){   VCC = voltage;  };
  //set value of top resistor for the voltage divider;
  void thermistor::setResistorDivider(float R){   resistance = R;   }; 
  //set rate of desired update times in number per second;
  void thermistor::setReadRate(float rate){   readRate = rate;    };     

//utility functions
  void thermistor::update(){
    intVal = analogRead(pin);
  }

  float thermistor::getVoltage(){
      float voltage;
      update();
      voltage = (VCC) * intVal / (pow(2,resolution) - 1);
      return voltage;
    }

  float thermistor::getAverageVoltage(int averages, int waitTime){
    float averagedVoltage = 0;
    for(int i = 0; i < averages; i++){
      averagedVoltage += getVoltage();
      delay(waitTime);
    }
    averagedVoltage = averagedVoltage/averages;
    return averagedVoltage;
  }
  
  float thermistor::getResistance(){
    float measuredResistance;
    float measuredVoltage = getVoltage();
    measuredResistance = resistance*measuredVoltage /(VCC - measuredVoltage);

    if (measuredResistance < 100*resistance){
      return measuredResistance;
    } else {
      return -1;
    }
  };

  float thermistor::getAverageResistance(int averages, int waitTime){
    float measuredResistance;
    float measuredVoltage = getAverageVoltage(averages,waitTime);
    measuredResistance = resistance*measuredVoltage /(VCC - measuredVoltage);

    if (measuredResistance < 100*resistance){
      return measuredResistance;
    } else {
      return -1;
    }

};
  
  float thermistor::getAverageTemperature(int averages, int waitTime){
       float averageResistance = getAverageResistance(averages,waitTime);
       float RtR = averageResistance / roomResistance;
       if (averageResistance > 100*resistance){
        return -1;
       }
       float T,a,b,c,d;
       
       a = 3.3530481e-3;
       b = 2.5420230e-4;
       c = 1.1431163e-6;
       d = -6.9383563e-8;

       float logRtR = log(RtR);
       T = 1/(a + b*logRtR+ c*pow(logRtR,2) + d*pow(logRtR,3));
       return T;
  };
}

#endif
