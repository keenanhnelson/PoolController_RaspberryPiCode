#include <stdio.h>
#include <wiringPi.h>


class Stepper{
	int pinActivate, pinSignal, pinDirection, pinOrigin, OriginDirection;
	int PosDir;
	public:
	int StepLoc;
	Stepper(int pActivate, int pSignal, int pDirection, int pOrigin, int OriginDir){
		pinActivate = pActivate;
		pinSignal = pSignal;
		pinDirection = pDirection;
		pinOrigin = pOrigin;
		OriginDirection = OriginDir;
		PosDir = !OriginDir;
		StepLoc = 0;
		pinMode(pinSignal, OUTPUT); 
		pinMode(pinActivate, OUTPUT);
		pinMode(pinDirection, OUTPUT);
		digitalWrite(pinActivate, LOW);
	}
	void Move(int Direction, int Steps){
		digitalWrite(pinDirection, Direction);//direction 
		digitalWrite(pinActivate, HIGH);//turn on driver
		delay(100);//wait for driver to fully turn on 

		printf("Direction = %i\n", Direction);
		printf("PosDir = %i\n", PosDir);
		for(int i=0; i<Steps; i++){
			digitalWrite(pinSignal, HIGH);
			delayMicroseconds(1000);
			digitalWrite(pinSignal, LOW);
			delayMicroseconds(1000);
			if(Direction == PosDir){
				StepLoc++;
			}else{
				StepLoc--;
			}
		}
		printf("Step Absolute Location = %i\n", StepLoc);
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
			StepLoc = 0;
		}
		printf("How many steps it took to get to origin = %i\n", count);
		digitalWrite(pinActivate, LOW);//put driver to sleep 
	}
};

class Servo{
	int pinSignal, pinOrigin, OriginDirection;
	public:
	static const int Right=0, Left=1, Backward=0, Forward=1, Up=1, Down=0;
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
	private:
		int X, Y;
	char ButtonName[8][20]= {"1"  ,"2"    ,"3"   ,"Plus","Menu","Right","Left","Minus"};
	int XButtonLoc[8] = 	{3300 ,7000   ,10700 ,7000  ,7000  ,5000   ,9000  ,7000};
	int YButtonLoc[8] = 	{1500 ,1500   ,1500  ,4500  ,6500  ,6500   ,6500  ,8700};
	int UpDownServo[8] = 	{1400 ,1400   ,1400  ,1400  ,1200  ,1200   ,1200  ,1200};
	public:
		static const int Right=0, Left=1, Backward=0, Forward=1, Up=1, Down=0;
		int FirstB=0,SecondB=1,ThirdB=2,PlusB=3,MenuB=4,RightB=5,LeftB=6,MinusB=7;
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
		void PressButtonFromOrigin(int i){
			printf("Returning to Origin\n");
			ResetToOrigin();
			printf("Going to press button %s\n", ButtonName[i]);
			FB->Move(Forward, YButtonLoc[i]);
			RL->Move(Left, XButtonLoc[i]);
			UD->Move(Down, UpDownServo[i]);
			printf("Button %s has been pressed\n", ButtonName[i]);
		}
		void PressButtonRelative(int i){
			printf("Relatively going to press Button %s\n", ButtonName[i]);
			Y = YButtonLoc[i] - FB->StepLoc;
			printf("Button1Relative Y needs to move %i steps\n", Y);
			if(Y >= 0){
				FB->Move(Forward, Y);
			}else{
				FB->Move(Backward, -Y);
			}
			X = XButtonLoc[i] - RL->StepLoc;
			printf("Button1Relative X needs to move %i steps\n", X);
			if(X >= 0){
				RL->Move(Left, X);
			}else{
				RL->Move(Right, -X);
			}
			printf("Finished Relative %s\n", ButtonName[i]);
		}
};

int main(void){

	wiringPiSetup();

	Stepper RightLeft(0, 6, 2, 7, Controller::Right);
	Stepper ForwardBackward(3, 4, 5, 8, Controller::Backward);
	Servo UpDown(1, 9, Controller::Up);

	//delay(5000);
	//while(1){
		ForwardBackward.Move(1, 10000);
	//}

	//Controller myControl(&RightLeft, &ForwardBackward, &UpDown);

	//myControl.PressButtonFromOrigin(myControl.SecondB);
	//myControl.ResetToOrigin();
	//myControl.PressButtonRelative(myControl.MenuB);
	//myControl.PressButtonRelative(myControl.FirstB);
	//for(int i=0; i<8; i++){
	//	myControl.PressButtonRelative(i);
	//}
}
