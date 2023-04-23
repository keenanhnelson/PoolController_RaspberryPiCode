#include "ServoPosition.hpp"
#include <wiringPi.h>

ServoPosition::ServoPosition(int pSignal){
	pinSignal = pSignal;
	pinMode(pinSignal, PWM_OUTPUT);
	pwmSetMode(PWM_MODE_MS);
	pwmSetClock(384); //clock at 50kHz (20us tick)
	pwmSetRange(1000); //range at 1000 ticks (20ms)
}

void ServoPosition::Move(int Position, int Duration){
	pwmWrite(pinSignal, Position);
	delay(Duration);
}

void ServoPosition::MoveToOrigin(){
	pwmWrite(pinSignal, 50);
	delay(500);
	pwmWrite(pinSignal, 0);//Make it so the servo doesn't twitch
}

void ServoPosition::LockPosition(){
	pwmWrite(pinSignal, 75);//75
	delay(500);
	pwmWrite(pinSignal, 0);//Make it so the servo doesn't twitch
}