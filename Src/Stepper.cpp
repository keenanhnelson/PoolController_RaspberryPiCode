#include "Stepper.hpp"
#include "pigpio.h"
#include <cstdio>

Stepper::Stepper(int pActivate, int pSignal, int pDirection, int pOrigin, int OriginDir,
			int InitialStepDelay){
	pinActivate = pActivate;
	pinSignal = pSignal;
	pinDirection = pDirection;
	pinOrigin = pOrigin;
	OriginDirection = OriginDir;
	PosDir = !OriginDir;
	StepDelay = InitialStepDelay;
	StepLoc = 0;
	gpioSetMode(pinSignal, PI_OUTPUT); 
	gpioSetMode(pinActivate, PI_OUTPUT);
	gpioSetMode(pinDirection, PI_OUTPUT);
	//gpioWrite(pinActivate, 0);
}
void Stepper::SetMoveSpeed(int SpeedToChangeTo){
	StepDelay = SpeedToChangeTo;
}
int Stepper::GetMoveSpeed(){
	return(StepDelay);
}
void Stepper::Move(int Direction, int Steps){
	gpioWrite(pinDirection, Direction);//direction 
	gpioWrite(pinActivate, 1);//turn on driver
	printf("waiting for stepper motor driver to turn on.\n");
	gpioDelay(100*1000);//wait for driver to fully turn on 
	printf("done waiting\n");


	printf("Direction = %i\n", Direction);
	printf("PosDir = %i\n", PosDir);
	for(int i=0; i<Steps; i++){
		gpioWrite(pinSignal, 1);
		gpioDelay(StepDelay);//1000
		gpioWrite(pinSignal, 0);
		gpioDelay(StepDelay);//1000
		if(Direction == PosDir){
			StepLoc++;
		}else{
			StepLoc--;
		}
	}
	printf("Step Absolute Location = %i\n", StepLoc);
	//gpioWrite(pinActivate, 0);//put driver to sleep 
}
void Stepper::MoveToOrigin(){
	gpioSetMode(pinOrigin, PI_INPUT);//Origin pin configure
	gpioSetPullUpDown(pinOrigin, PI_PUD_UP);

	gpioWrite(pinDirection, OriginDirection);//direction 
	gpioWrite(pinActivate, 1);//turn on driver
	gpioDelay(100*1000);//wait for driver to fully turn on 

	int count = 0;
	while(gpioRead(pinOrigin) == 1){
		gpioWrite(pinSignal, 1);
		gpioDelay(StepDelay);
		gpioWrite(pinSignal, 0);
		gpioDelay(StepDelay);
		count++;
		StepLoc = 0;
	}
	printf("How many steps it took to get to origin = %i\n", count);
	//gpioWrite(pinActivate, 0);//put driver to sleep 
}
void Stepper::TurnOff(){
	gpioWrite(pinActivate, 0);//put driver to sleep 
}
void Stepper::TurnOnStepperToStabilizePosition(){
	gpioWrite(pinActivate, 1);//Wake up driver. Used to stabilize position
	//gpioDelay(1000000);//Sleep for 1ms to stabilize position
	printf("stabilizing position\n");
	gpioDelay(200*1000);
	printf("stabilizing finished\n");
}
