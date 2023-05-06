#include <iostream>
#include "pigpio.h"
#include "Stepper.hpp"
#include "ServoPosition.hpp"
#include "Controller.hpp"
#include "Server.hpp"
#include "Config.hpp"

int main(int a, char **b){

	if(gpioInitialise() < 0) return 1;

	Stepper ForwardBackward(FB_ActivatePin, FB_SignalPin, 
							FB_DirectionPin, FB_EndStopPin, 
							Controller::Forward, FB_StepPeriod);
	Stepper RightLeft(RL_ActivatePin, RL_SignalPin, RL_DirectionPin, 
						RL_EndStopPin, Controller::Left, RL_StepPeriod);
	ServoPosition UpDown(UpDn_SignalPin);

	Controller myControl(&RightLeft, &ForwardBackward, 
							&UpDown, EjectRemotePin);

	Webcam Cam(LED1Pin, LED2Pin);
							
	Server myServer(&myControl, &Cam, ServerPort);
	myServer.Start();//Stay here forever

	// myControl.MoveToLockPosition();
	// int pwmValue;
	// while(1){
	// 	std::cin >> pwmValue;
	// 	if(pwmValue < 500 || pwmValue>2500){
	// 		break;
	// 	}
	// 	UpDown.Move(pwmValue, 2000);
	// }
	// myControl.TurnOffSteppers();
	// myControl.TurnOffServo();








	//The following code is for debugging only
	//std::cout << "Turning on steppers. Press enter to continue\n";
	//myControl.TurnOnSteppersToStabilizePosition();
	//
	//int ButtonPos = 0;
	//while(1){
	//	std::cout << "Choose a position from 0 to 7\n";
	//	std::cin >> ButtonPos;
	//	myControl.PressButtonFromOrigin(ButtonPos);
	//	myControl.MoveToLockPosition();
	//	myControl.TurnOffSteppers();
	//	myControl.TurnOffServo();
	//}

	return(0);
}
