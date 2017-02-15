#include <stdio.h>
#include <wiringPi.h>

typedef struct{
	int Activate, Signal, Direction, Origin;
}StepperConfigPins;
typedef struct{
	int Signal, Origin;
}ServoConfigPins;
int ServoSignalUD = 1; 
int Right = 0, Left = 1, Backward = 0, Forward = 1, Up = 1, Down = 0;

void StepperConfigure(StepperConfigPins pin);
void StepperMove(StepperConfigPins pin, int Direction, int Steps);
void StepperMoveToOrigin(StepperConfigPins pin, int Direction);
void ServoMove(ServoConfigPins pin, int Direction, int Duration);
void PressButton1FromOrigin(StepperConfigPins pinFB, StepperConfigPins pinRL, 
								ServoConfigPins pinServo);
void PressButton2FromOrigin(StepperConfigPins pinFB, StepperConfigPins pinRL, 
								ServoConfigPins pinServo);
void PressButton3FromOrigin(StepperConfigPins pinFB, StepperConfigPins pinRL, 
								ServoConfigPins pinServo);
void ResetToOrigin(StepperConfigPins pinFB, StepperConfigPins pinRL, 
					ServoConfigPins pinServo);
void PressPlusButtonFromOrigin(StepperConfigPins pinFB, StepperConfigPins pinRL, 
								ServoConfigPins pinServo);
void PressMenuButtonFromOrigin(StepperConfigPins pinFB, StepperConfigPins pinRL, 
								ServoConfigPins pinServo);
void PressRightArrowButtonFromOrigin(StepperConfigPins pinFB, StepperConfigPins pinRL, 
								ServoConfigPins pinServo);
void PressLeftArrowButtonFromOrigin(StepperConfigPins pinFB, StepperConfigPins pinRL, 
								ServoConfigPins pinServo);
void PressMinusButtonFromOrigin(StepperConfigPins pinFB, StepperConfigPins pinRL, 
								ServoConfigPins pinServo);

int main(void){
	StepperConfigPins RL = {0, 6, 2, 7};
	StepperConfigPins FB = {3, 4, 5, 8};
	ServoConfigPins Servo = {1, 9};

	//int StepperActivateRL = 0, StepperSignalRL = 6, StepperDirectionRL = 2;
	//int StepperActivateFB = 3, StepperSignalFB = 4, StepperDirectionFB = 5;
	//int OriginRight = 7, OriginBottom = 8;

	wiringPiSetup();

	StepperConfigure(RL);
	StepperConfigure(FB);

	ResetToOrigin(FB, RL, Servo);
	//PressMinusButtonFromOrigin(FB, RL, Servo);
	//PressMenuButtonFromOrigin(FB, RL, Servo);
	//PressButton3FromOrigin(FB, RL, Servo);
	//ResetToOrigin(FB, RL, Servo);

	//StepperMove(FB, Backward, 500);
	//StepperMove(RL, Right, 2000);

	//ServoMove(Servo, Down, 1200);//1s = 100 14
	//ServoMove(Servo, Up, 10000);//Reset to top

	//ResetToOrigin(FB, RL, Servo);

	//PressButton1FromOrigin(FB, RL, Servo);
	//ResetToOrigin(FB, RL, Servo);

	//PressButton2FromOrigin(FB, RL, Servo);
	//ResetToOrigin(FB, RL, Servo);

	//PressButton3FromOrigin(FB, RL, Servo);
	//ResetToOrigin(FB, RL, Servo);
}

void PressButton1FromOrigin(StepperConfigPins pinFB, StepperConfigPins pinRL, 
								ServoConfigPins pinServo){
	StepperMove(pinFB, Forward, 1500);
	StepperMove(pinRL, Left, 3300);
	ServoMove(pinServo, Down, 1400);//1s = 100
}
void PressButton2FromOrigin(StepperConfigPins pinFB, StepperConfigPins pinRL, 
								ServoConfigPins pinServo){
	StepperMove(pinFB, Forward, 1500);
	StepperMove(pinRL, Left, 7000);
	ServoMove(pinServo, Down, 1400);//1s = 100
}
void PressButton3FromOrigin(StepperConfigPins pinFB, StepperConfigPins pinRL, 
								ServoConfigPins pinServo){
	StepperMove(pinFB, Forward, 1500);
	StepperMove(pinRL, Left, 10700);
	ServoMove(pinServo, Down, 1400);//1s = 100
}



void PressPlusButtonFromOrigin(StepperConfigPins pinFB, StepperConfigPins pinRL, 
								ServoConfigPins pinServo){
	StepperMove(pinFB, Forward, 4500);
	StepperMove(pinRL, Left, 7000);
	ServoMove(pinServo, Down, 1400);//1s = 100
}
void PressMenuButtonFromOrigin(StepperConfigPins pinFB, StepperConfigPins pinRL, 
								ServoConfigPins pinServo){
	StepperMove(pinFB, Forward, 6500);
	StepperMove(pinRL, Left, 7000);
	ServoMove(pinServo, Down, 1200);//1s = 100
}
void PressRightArrowButtonFromOrigin(StepperConfigPins pinFB, StepperConfigPins pinRL, 
								ServoConfigPins pinServo){
	StepperMove(pinFB, Forward, 6500);
	StepperMove(pinRL, Left, 5000);
	ServoMove(pinServo, Down, 1200);//1s = 100
}
void PressLeftArrowButtonFromOrigin(StepperConfigPins pinFB, StepperConfigPins pinRL, 
								ServoConfigPins pinServo){
	StepperMove(pinFB, Forward, 6500);
	StepperMove(pinRL, Left, 9000);
	ServoMove(pinServo, Down, 1200);//1s = 100
}
void PressMinusButtonFromOrigin(StepperConfigPins pinFB, StepperConfigPins pinRL, 
								ServoConfigPins pinServo){
	StepperMove(pinFB, Forward, 8700);
	StepperMove(pinRL, Left, 7000);
	ServoMove(pinServo, Down, 1200);//1s = 100
}





void ResetToOrigin(StepperConfigPins pinFB, StepperConfigPins pinRL, 
					ServoConfigPins pinServo){
	ServoMove(pinServo, Up, 123456);//big number to make sure reset to top
	StepperMoveToOrigin(pinRL, Right);
	StepperMoveToOrigin(pinFB, Backward);
}

void StepperConfigure(StepperConfigPins pin){
	pinMode(pin.Signal, OUTPUT); 
	pinMode(pin.Activate, OUTPUT);
	pinMode(pin.Direction, OUTPUT);
	digitalWrite(pin.Activate, LOW);
}

void StepperMove(StepperConfigPins pin, int Direction, int Steps){
	digitalWrite(pin.Direction, Direction);//direction 
	digitalWrite(pin.Activate, HIGH);//turn on driver
	delay(100);//wait for driver to fully turn on 

	for(int i=0; i<Steps; i++){
		digitalWrite(pin.Signal, HIGH);
		delayMicroseconds(1000);
		digitalWrite(pin.Signal, LOW);
		delayMicroseconds(1000);
	}

	digitalWrite(pin.Activate, LOW);//put driver to sleep 
}

void StepperMoveToOrigin(StepperConfigPins pin, int Direction){
	pinMode(pin.Origin, INPUT);//Origin pin configure
	pullUpDnControl(pin.Origin, PUD_UP);

	digitalWrite(pin.Direction, Direction);//direction 
	digitalWrite(pin.Activate, HIGH);//turn on driver
	delay(100);//wait for driver to fully turn on 

	int count = 0;
	while(digitalRead(pin.Origin) == 1){
		digitalWrite(pin.Signal, HIGH);
		delayMicroseconds(1000);
		digitalWrite(pin.Signal, LOW);
		delayMicroseconds(1000);
		count++;
	}
	printf("count = %i\n", count);
	digitalWrite(pin.Activate, LOW);//put driver to sleep 
}

void ServoMove(ServoConfigPins pin, int Direction, int Duration){
	pinMode(pin.Origin, INPUT);
	pullUpDnControl(pin.Origin, PUD_UP);
	//if((digitalRead(pin.Origin) != 0) && (Direction != Up)){
		if(Direction == 0){
			pwmWrite(pin.Signal, 100);
		}
		if(Direction == 1){
			pwmWrite(pin.Signal, 50);
		}
		pinMode(pin.Signal, PWM_OUTPUT);
		pwmSetMode(PWM_MODE_MS);
		pwmSetClock(384); //clock at 50kHz (20us tick)
		pwmSetRange(1000); //range at 1000 ticks (20ms)
		pinMode(pin.Origin, INPUT);
		for(int i=0; i<Duration; i++){

			if((digitalRead(pin.Origin) == 0) && (Direction == Up)){
				break;
			}else{
				delay(10);
			}
		}
	//}
	pinMode(pin.Signal, INPUT);//Disable PWM
}
