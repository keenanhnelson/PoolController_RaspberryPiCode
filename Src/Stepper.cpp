#include "Stepper.hpp"
#include <wiringPi.h>
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
	pinMode(pinSignal, OUTPUT); 
	pinMode(pinActivate, OUTPUT);
	pinMode(pinDirection, OUTPUT);
	//digitalWrite(pinActivate, LOW);
}
void Stepper::SetMoveSpeed(int SpeedToChangeTo){
	StepDelay = SpeedToChangeTo;
}
int Stepper::GetMoveSpeed(){
	return(StepDelay);
}
void Stepper::Move(int Direction, int Steps){
	digitalWrite(pinDirection, Direction);//direction 
	digitalWrite(pinActivate, HIGH);//turn on driver
	printf("waiting for stepper motor driver to turn on.\n");
	delay(100);//wait for driver to fully turn on 
	printf("done waiting\n");


	printf("Direction = %i\n", Direction);
	printf("PosDir = %i\n", PosDir);
	for(int i=0; i<Steps; i++){
		digitalWrite(pinSignal, HIGH);
		delayMicroseconds(StepDelay);//1000
		digitalWrite(pinSignal, LOW);
		delayMicroseconds(StepDelay);//1000
		if(Direction == PosDir){
			StepLoc++;
		}else{
			StepLoc--;
		}
	}
	printf("Step Absolute Location = %i\n", StepLoc);
	//digitalWrite(pinActivate, LOW);//put driver to sleep 
}
void Stepper::MoveToOrigin(){
	pinMode(pinOrigin, INPUT);//Origin pin configure
	pullUpDnControl(pinOrigin, PUD_UP);

	digitalWrite(pinDirection, OriginDirection);//direction 
	digitalWrite(pinActivate, HIGH);//turn on driver
	delay(100);//wait for driver to fully turn on 

	int count = 0;
	while(digitalRead(pinOrigin) == 1){
		digitalWrite(pinSignal, HIGH);
		delayMicroseconds(StepDelay);
		digitalWrite(pinSignal, LOW);
		delayMicroseconds(StepDelay);
		count++;
		StepLoc = 0;
	}
	printf("How many steps it took to get to origin = %i\n", count);
	//digitalWrite(pinActivate, LOW);//put driver to sleep 
}
void Stepper::TurnOff(){
	digitalWrite(pinActivate, LOW);//put driver to sleep 
}
void Stepper::TurnOnStepperToStabilizePosition(){
	digitalWrite(pinActivate, HIGH);//Wake up driver. Used to stabilize position
	//delayMicroseconds(1000000);//Sleep for 1ms to stabilize position
	printf("stabilizing position\n");
	delay(200);
	printf("stabilizing finished\n");
}
