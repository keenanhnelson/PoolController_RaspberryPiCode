#include <stdio.h>
#include <wiringPi.h>

int Right = 0, Left = 1, Backward = 0, Forward = 1, Up = 1, Down = 0;

class Stepper{
	int pinActivate, pinSignal, pinDirection, pinOrigin, OriginDirection;
	public:
	Stepper(int pActivate, int pSignal, int pDirection, int pOrigin, int OriginDir){
		pinActivate = pActivate;
		pinSignal = pSignal;
		pinDirection = pDirection;
		pinOrigin = pOrigin;
		OriginDirection = OriginDir;
		pinMode(pinSignal, OUTPUT); 
		pinMode(pinActivate, OUTPUT);
		pinMode(pinDirection, OUTPUT);
		digitalWrite(pinActivate, LOW);
	}
	void Move(int Direction, int Steps){
		digitalWrite(pinDirection, Direction);//direction 
		digitalWrite(pinActivate, HIGH);//turn on driver
		delay(100);//wait for driver to fully turn on 

		for(int i=0; i<Steps; i++){
			digitalWrite(pinSignal, HIGH);
			delayMicroseconds(1000);
			digitalWrite(pinSignal, LOW);
			delayMicroseconds(1000);
		}

		digitalWrite(pinActivate, LOW);//put driver to sleep 
	}
	void MoveToOrigin(){
		pinMode(pinOrigin, INPUT);//Origin pin configure
		pullUpDnControl(pinOrigin, PUD_UP);

		digitalWrite(pinDirection, OriginDirection);//direction 
		digitalWrite(pinActivate, HIGH);//turn on driver
		delay(100);//wait for driver to fully turn on 

		int count = 0;
		while(digitalRead(pinOrigin) == 1){
			digitalWrite(pinSignal, HIGH);
			delayMicroseconds(1000);
			digitalWrite(pinSignal, LOW);
			delayMicroseconds(1000);
			count++;
		}
		printf("count = %i\n", count);
		digitalWrite(pinActivate, LOW);//put driver to sleep 
	}
};

class Servo{
	int pinSignal, pinOrigin, OriginDirection;
	public:
	Servo(int pSignal, int pOrigin, int OriginDir){
		pinSignal = pSignal;
		pinOrigin = pOrigin;
		OriginDirection = OriginDir;
	}

	void Move(int Direction, int Duration){
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

	void MoveToOrigin(){
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
};

class Controller{
	public:
		Stepper *RL, *FB;
		Servo *UD;
		Controller(Stepper *inputRL, Stepper *inputFB, Servo *inputUD){
			RL = inputRL;
			FB = inputFB;
			UD = inputUD;
		}
		void ResetToOrigin(){
			UD->MoveToOrigin();
			RL->MoveToOrigin();
			FB->MoveToOrigin();
		}
		void PressButton1FromOrigin(){
			FB->Move(Forward, 1500);
			RL->Move(Left, 3300);
			UD->Move(Down, 1400);
		}
		void PressButton2FromOrigin(){
			FB->Move(Forward, 1500);
			RL->Move(Left, 7000);
			UD->Move(Down, 1400);
		}
		void PressButton3FromOrigin(){
			FB->Move(Forward, 1500);
			RL->Move(Left, 10700);
			UD->Move(Down, 1400);
		}
		void PressPlusButtonFromOrigin(){
			FB->Move(Forward, 4500);
			RL->Move(Left, 7000);
			UD->Move(Down, 1400);
		}
		void PressMenuButtonFromOrigin(){
			FB->Move(Forward, 6500);
			RL->Move(Left, 7000);
			UD->Move(Down, 1200);
		}
		void PressRightArrowButtonFromOrigin(){
			FB->Move(Forward, 6500);
			RL->Move(Left, 5000);
			UD->Move(Down, 1200);
		}
		void PressLeftArrowButtonFromOrigin(){
			FB->Move(Forward, 6500);
			RL->Move(Left, 9000);
			UD->Move(Down, 1200);
		}
		void PressMinusButtonFromOrigin(){
			FB->Move(Forward, 8700);
			RL->Move(Left, 7000);
			UD->Move(Down, 1200);
		}
};

int main(void){

	wiringPiSetup();

	Stepper RightLeft(0, 6, 2, 7, Right);
	Stepper ForwardBackward(3, 4, 5, 8, Backward);
	Servo UpDown(1, 9, Up);

	Controller myControl(&RightLeft, &ForwardBackward, &UpDown);

	myControl.ResetToOrigin();
	myControl.PressMinusButtonFromOrigin();
	myControl.ResetToOrigin();

}
