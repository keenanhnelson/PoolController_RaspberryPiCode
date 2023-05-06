#include "ServoContinuous.hpp"
#include "pigpio.h"

ServoContinuous::ServoContinuous(int pSignal, int pOrigin, int OriginDir){
	pinSignal = pSignal;
	pinOrigin = pOrigin;
	OriginDirection = OriginDir;
}

void ServoContinuous::Move(int Direction, int Duration){
	gpioSetMode(pinOrigin, PI_INPUT);
	gpioSetPullUpDown(pinOrigin, PI_PUD_UP);
	if(!((gpioRead(pinOrigin) == 0) && (Direction == Up))){//prevent twitch
		if(Direction == 0){
			gpioServo(pinSignal, 2500);//Originally 100
		}
		if(Direction == 1){
			gpioServo(pinSignal, 500);//Originally 50
		}
		for(int i=0; i<Duration; i++){
			if((gpioRead(pinOrigin) == 0) && (Direction == Up)){
				break;
			}else{
				gpioDelay(10*1000);
			}
		}
	}
	gpioServo(pinSignal, 0);//Disable PWM
}

void ServoContinuous::MoveToOrigin(){
	gpioSetMode(pinOrigin, PI_INPUT);
	gpioSetPullUpDown(pinOrigin, PI_PUD_UP);
	if(!((gpioRead(pinOrigin) == 0))){//prevent twitch
		if(OriginDirection == 0){
			gpioServo(pinSignal, 2500);//Originally 100
		}
		if(OriginDirection == 1){
			gpioServo(pinSignal, 500);//Originally 100
		}
		while(gpioRead(pinOrigin) == 1){
			gpioDelay(10*1000);
		}
	}
	gpioServo(pinSignal, 0);//Disable PWM
}
