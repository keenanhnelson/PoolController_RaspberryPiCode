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
	pwmWrite(pinSignal, OriginPos);
	delay(500);
	TurnOff();//Make it so the servo doesn't twitch
}

void ServoPosition::TurnOff(){
	pwmWrite(pinSignal, 0);//Make it so the servo doesn't twitch
}