#include <iostream>
#include "pigpio.h"
#include "Stepper.hpp"
#include "ServoPosition.hpp"
#include "Controller.hpp"
#include "Server.hpp"
#include "Config.hpp"
#include "ServerInfoPrivate.hpp"

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




	// //The following code is for debugging stepper motor speed
	// myControl.TurnOnSteppersToStabilizePosition();
	// UpDown.MoveToOrigin();
	// for(int i=1300; i<2000; i+=50){
	// 	std::cout << "Setting speed to " << i << "\n" << std::flush;
	// 	RightLeft.SetMoveSpeed(i);
	// 	RightLeft.Move(Controller::Right, 350);
	// 	RightLeft.MoveToOrigin();
	// }
	// myControl.TurnOffSteppers();
	// myControl.TurnOffServo();



	//The following code is for debugging servo position
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
