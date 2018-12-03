#include "ServoContinuous.hpp"
#include <wiringPi.h>

ServoContinuous::ServoContinuous(int pSignal, int pOrigin, int OriginDir){
	pinSignal = pSignal;
	pinOrigin = pOrigin;
	OriginDirection = OriginDir;
}

void ServoContinuous::Move(int Direction, int Duration){
	pinMode(pinOrigin, INPUT);
	pullUpDnControl(pinOrigin, PUD_UP);
	if(!((digitalRead(pinOrigin) == 0) && (Direction == Up))){//prevent twitch
		if(Direction == 0){
			pwmWrite(pinSignal, 100);
		}
		if(Direction == 1){
			pwmWrite(pinSignal, 50);
		}
		pinMode(pinSignal, PWM_OUTPUT);
		pwmSetMode(PWM_MODE_MS);
		pwmSetClock(384); //clock at 50kHz (20us tick)
		pwmSetRange(1000); //range at 1000 ticks (20ms)
		pinMode(pinOrigin, INPUT);
		for(int i=0; i<Duration; i++){
			if((digitalRead(pinOrigin) == 0) && (Direction == Up)){
				break;
			}else{
				delay(10);
			}
		}
	}
	pinMode(pinSignal, INPUT);//Disable PWM
}

void ServoContinuous::MoveToOrigin(){
	pinMode(pinOrigin, INPUT);
	pullUpDnControl(pinOrigin, PUD_UP);
	if(!((digitalRead(pinOrigin) == 0))){//prevent twitch
		if(OriginDirection == 0){
			pwmWrite(pinSignal, 100);
		}
		if(OriginDirection == 1){
			pwmWrite(pinSignal, 50);
		}
		pinMode(pinSignal, PWM_OUTPUT);
		pwmSetMode(PWM_MODE_MS);
		pwmSetClock(384); //clock at 50kHz (20us tick)
		pwmSetRange(1000); //range at 1000 ticks (20ms)
		pinMode(pinOrigin, INPUT);
		while(digitalRead(pinOrigin) == 1){
			delay(10);
		}
	}
	pinMode(pinSignal, INPUT);//Disable PWM
}
