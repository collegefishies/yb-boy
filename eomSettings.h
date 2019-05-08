#define EOMPROGS 9
#define EOMBAK "TEMP_PI"
menu eomSettings;

extern temperatureController::TemperatureController eom;

namespace eomMenu {

void setEomVar(float& x, String prompt){
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
	eom.saveConfig(EOMBAK);
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

void setFeedbackTime(){setEomVar(eom.feedbackTime, "Input feedback time:\n");}
void setG(){setEomVar(eom.lockbox.G, "Input G:\n");}
void setP(){setEomVar(eom.lockbox.P, "Input P:\n");}
void setI(){setEomVar(eom.lockbox.I, "Input I (1/s):\n");}
void setSetpoint(){setEomVar(eom.lockbox.setpoint, "Input setpoint (C):\n");};
void setOutputOffset(){setEomVar(eom.lockbox.outputOffset, "Input outputOffset:\n");}

void switchFeedback(){
	eom.lockbox.G *= -1;
	eom.saveConfig(EOMBAK);
}


void toggleFeedback(){
	eom.lockbox.locked = !eom.lockbox.locked;
}

void zeroIntegrator(){
	eom.lockbox.integral = 0;
	eom.saveConfig(EOMBAK);
}

void printFreeRam(){
	lcd.println("Free RAM is: ");
	lcd.println(freeMemory());
	wait();
}
void printSettings(){
	char buff[200];
	sprintf(buff, 
		"Total Gain: %.3e\nProportional: %.3f\nIntegral Gain: %.3e\nOutput Offset: %.3f\nSetpoint (C): %.3f\n",
		eom.lockbox.G, eom.lockbox.P,
		eom.lockbox.I, eom.lockbox.outputOffset,
		eom.lockbox.setpoint);
	lcd.println(buff);
	wait();
}

String settingsItems[EOMPROGS]   = {
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

void (*settingsProgs[EOMPROGS])()= {
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