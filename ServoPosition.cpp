#include "ServoPosition.hpp"
#include <wiringPi.h>

ServoPosition::ServoPosition(int pSignal){
	pinSignal = pSignal;
	//pwmWrite(pinSignal, 90);
	pinMode(pinSignal, PWM_OUTPUT);
	pwmSetMode(PWM_MODE_MS);
	pwmSetClock(384); //clock at 50kHz (20us tick)
	pwmSetRange(1000); //range at 1000 ticks (20ms)
}

void ServoPosition::Move(int Position, int Duration){
	pwmWrite(pinSignal, Position);
	//pinMode(pinSignal, PWM_OUTPUT);
	//pwmSetMode(PWM_MODE_MS);
	//pwmSetClock(384); //clock at 50kHz (20us tick)
	//pwmSetRange(1000); //range at 1000 ticks (20ms)
	delay(Duration);
	//pinMode(pinSignal, INPUT);//Disable PWM
}

void ServoPosition::MoveToOrigin(){
	pwmWrite(pinSignal, 50);
	//pinMode(pinSignal, PWM_OUTPUT);
	//pwmSetMode(PWM_MODE_MS);
	//pwmSetClock(384); //clock at 50kHz (20us tick)
	//pwmSetRange(1000); //range at 1000 ticks (20ms)
	delay(500);
	//pinMode(pinSignal, INPUT);//Disable PWM
}

void ServoPosition::LockPosition(){
	pwmWrite(pinSignal, 75);//75
	delay(500);
}