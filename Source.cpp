#include <iostream>
#include <wiringPi.h>
#include "Stepper.hpp"
#include "ServoPosition.hpp"
#include "Controller.hpp"
#include "Server.hpp"
#include "Config.hpp"

int main(int a, char **b){

	wiringPiSetup();

	Stepper ForwardBackward(FB_ActivatePin, FB_SignalPin, 
							FB_DirectionPin, FB_EndStopPin, 
							Controller::Forward, FB_StepPeriod);
	Stepper RightLeft(RL_ActivatePin, RL_SignalPin, RL_DirectionPin, 
						RL_EndStopPin, Controller::Left, RL_StepPeriod);
	ServoPosition UpDown(UpDn_SignalPin);

	Controller myControl(&RightLeft, &ForwardBackward, 
							&UpDown, EjectRemotePin);
	Server myServer(&myControl, ServerPort, LED1Pin, LED2Pin);
	myServer.Start();

	////Let user decide what button to press
	//Controller myControl(&RightLeft, &ForwardBackward, &UpDown, 13);
	//while(1){
	//	cout << "Enter desire button number\n";
	//	int ButtonNumber;
	//	cin >> ButtonNumber;
	//	if( !((ButtonNumber >= 0) && (ButtonNumber <= 7)) ){
	//		break;
	//	}
	//	myControl.PressButtonFromOrigin(ButtonNumber);
	//}
	//myControl.TurnOffSteppers();


	return(0);
}
