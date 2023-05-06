#include "ServoPosition.hpp"
#include "pigpio.h"

ServoPosition::ServoPosition(int pSignal){
	pinSignal = pSignal;
}

void ServoPosition::Move(int Position, int Duration){
	gpioServo(pinSignal, Position);
	gpioDelay(Duration*1000);
}

void ServoPosition::MoveToOrigin(){
	gpioServo(pinSignal, OriginPos);
	gpioDelay(500*1000);
	TurnOff();//Make it so the servo doesn't twitch
}

void ServoPosition::TurnOff(){
	gpioServo(pinSignal, 0);//Make it so the servo doesn't twitch
}