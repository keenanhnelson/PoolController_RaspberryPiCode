#include "Controller.hpp"
#include <wiringPi.h>

Controller::Controller(Stepper *inputRL, Stepper *inputFB, ServoPosition *inputUD, 
			int inputEjectButtonPin){
	RL = inputRL;
	FB = inputFB;
	UD = inputUD;
	EjectButtonThread = std::thread(&Controller::HandleEjectStatus, this,
								inputEjectButtonPin);
	EjectButtonThread.detach();
	
}
void Controller::HandleEjectStatus(int EjectButtonPin){//Thread use only
	//Setup eject button
	pinMode(EjectButtonPin, INPUT);
	pullUpDnControl(EjectButtonPin, PUD_UP);
	bool LockPositionState = 0;

	while(1){
		if(digitalRead(EjectButtonPin) == 0 && LockPositionState == 0){
			MotorControlMutex.lock();
			MoveToUnlockPosition();
			LockPositionState = 1;
			TurnOffSteppers();
			MotorControlMutex.unlock();
		}
		if(digitalRead(EjectButtonPin) == 0 && LockPositionState == 1){
			MotorControlMutex.lock();
			MoveToLockPosition();
			LockPositionState = 0;
			TurnOffSteppers();
			MotorControlMutex.unlock();
		}
	}
}
void Controller::ResetToOrigin(){
	UD->MoveToOrigin();
	FB->MoveToOrigin();
	RL->MoveToOrigin();
}
void Controller::MoveToLockPosition(){
	ResetToOrigin();
	FB->Move(Controller::Backward, 12);
	UD->LockPosition();
}
void Controller::MoveToUnlockPosition(){
	UD->MoveToOrigin();
	//ResetToOrigin();
	//FB->Move(Controller::Backward, 220);
}
void Controller::TurnOffSteppers(){
	RL->TurnOff();
	FB->TurnOff();
}
void Controller::TurnOnSteppersToStabilizePosition(){
	RL->TurnOnStepperToStabilizePosition();
	FB->TurnOnStepperToStabilizePosition();
}
void Controller::PressButtonFromOrigin(int i){
	MotorControlMutex.lock();
	TurnOnSteppersToStabilizePosition();
	printf("Returning to Origin\n");
	ResetToOrigin();
	printf("Going to press button %s\n", ButtonName[i]);
	FB->Move(Backward, YButtonLoc[i]);
	RL->Move(Right, XButtonLoc[i]);
	UD->Move(UpDownServo[i], 700);
	UD->MoveToOrigin();
	printf("Button %s has been pressed\n", ButtonName[i]);
	MotorControlMutex.unlock();
}
void Controller::PressButtonRelative(int i){
	MotorControlMutex.lock();
	UD->MoveToOrigin();//Make sure servo is all the way up
	printf("Button %s has been pressed\n", ButtonName[i]);
	printf("Relatively going to press Button %s\n", ButtonName[i]);
	Y = YButtonLoc[i] - FB->StepLoc;
	printf("Button1Relative Y needs to move %i steps\n", Y);
	if(Y >= 0){
		FB->Move(Backward, Y);
	}else{
		FB->Move(Forward, -Y);
	}
	X = XButtonLoc[i] - RL->StepLoc;
	printf("Button1Relative X needs to move %i steps\n", X);
	if(X >= 0){
		RL->Move(Right, X);
	}else{
		RL->Move(Left, -X);
	}
	printf("Finished Relative %s\n", ButtonName[i]);
	MotorControlMutex.unlock();
}
