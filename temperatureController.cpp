#ifndef temperatureController_cpp
#define temperatureController_cpp

#include <Arduino.h>
#include "temperatureController.h"
#include <math.h>

namespace temperatureController{
/*
 *  Temperature Controller
 */

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
