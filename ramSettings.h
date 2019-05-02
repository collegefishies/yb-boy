#define RAMPROGS 10
#define RAMBAK "RAM_PI"
menu ramSettings;

extern temperatureController::TemperatureController ram;

namespace ramMenu {

void setRamVar(float& x, String prompt){
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
	ram.saveConfig(RAMBAK);
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

void setFeedbackTime(){setRamVar(ram.feedbackTime, "Input feedback time:\n");}
void setG(){setRamVar(ram.lockbox.G, "Input G:\n");}
void setP(){setRamVar(ram.lockbox.P, "Input P:\n");}
void setI(){setRamVar(ram.lockbox.I, "Input I (1/s):\n");}
void setSetpoint(){setRamVar(ram.lockbox.setpoint, "Input setpoint (C):\n");};
void setOutputOffset(){setRamVar(ram.lockbox.outputOffset, "Input outputOffset:\n");}

void switchFeedback(){
	ram.lockbox.G *= -1;
	ram.saveConfig(RAMBAK);
}


void toggleFeedback(){
	ram.lockbox.locked = !ram.lockbox.locked;
}

void zeroIntegrator(){
	ram.lockbox.integral = 0;
	ram.saveConfig(RAMBAK);
}


void printSettings(){
	char buff[200];
	sprintf(buff, 
		"Total Gain: %.3e\nProportional: %.3f\nIntegral Gain: %.3e\nOutput Offset: %.3f\nSetpoint (C): %.3f\n",
		ram.lockbox.G, ram.lockbox.P,
		ram.lockbox.I, ram.lockbox.outputOffset,
		ram.lockbox.setpoint);
	lcd.println(buff);
	wait();
}

String settingsItems[RAMPROGS]   = {
	"Toggle Feedback",
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

void (*settingsProgs[RAMPROGS])()= {
	toggleFeedback,
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