#include <stdio.h>
#include <wiringPi.h>

int Right = 0, Left = 1, Backward = 0, Forward = 1, Up = 1, Down = 0;

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
		void PressButton1Relative(){
			Y = 1500 - FB->StepLoc;
			printf("Button1Relative Y needs to move %i steps\n", Y);
			if(Y >= 0){
				FB->Move(Forward, Y);
			}else{
				FB->Move(Backward, -Y);
			}
			X = 3300 - RL->StepLoc;
			printf("Button1Relative X needs to move %i steps\n", X);
			if(X >= 0){
				RL->Move(Left, X);
			}else{
				RL->Move(Right, -X);
			}

		}
		void PressButton2FromOrigin(){
			FB->Move(Forward, 1500);
			RL->Move(Left, 7000);
			UD->Move(Down, 1400);
		}
		void PressButton2Relative(){
			Y = 1500 - FB->StepLoc;
			printf("Button2Relative Y needs to move %i steps\n", Y);
			if(Y >= 0){
				FB->Move(Forward, Y);
			}else{
				FB->Move(Backward, -Y);
			}
			X = 7000 - RL->StepLoc;
			printf("Button2Relative X needs to move %i steps\n", X);
			if(X >= 0){
				RL->Move(Left, X);
			}else{
				RL->Move(Right, -X);
			}

		}
		void PressButton3FromOrigin(){
			FB->Move(Forward, 1500);
			RL->Move(Left, 10700);
			UD->Move(Down, 1400);
		}
		void PressButton3Relative(){
			Y = 1500 - FB->StepLoc;
			printf("Button3Relative Y needs to move %i steps\n", Y);
			if(Y >= 0){
				FB->Move(Forward, Y);
			}else{
				FB->Move(Backward, -Y);
			}
			X = 10700 - RL->StepLoc;
			printf("Button3Relative X needs to move %i steps\n", X);
			if(X >= 0){
				RL->Move(Left, X);
			}else{
				RL->Move(Right, -X);
			}

		}
		void PressPlusButtonFromOrigin(){
			FB->Move(Forward, 4500);
			RL->Move(Left, 7000);
			UD->Move(Down, 1400);
		}
		void PressPlusButtonRelative(){
			Y = 4500 - FB->StepLoc;
			printf("PlusButtonRelative Y needs to move %i steps\n", Y);
			if(Y >= 0){
				FB->Move(Forward, Y);
			}else{
				FB->Move(Backward, -Y);
			}
			X = 7000 - RL->StepLoc;
			printf("PlusButtonRelative X needs to move %i steps\n", X);
			if(X >= 0){
				RL->Move(Left, X);
			}else{
				RL->Move(Right, -X);
			}

		}
		void PressMenuButtonFromOrigin(){
			FB->Move(Forward, 6500);
			RL->Move(Left, 7000);
			UD->Move(Down, 1200);
		}
		void PressMenuButtonRelative(){
			Y = 6500 - FB->StepLoc;
			printf("MenuButtonRelative Y needs to move %i steps\n", Y);
			if(Y >= 0){
				FB->Move(Forward, Y);
			}else{
				FB->Move(Backward, -Y);
			}
			X = 7000 - RL->StepLoc;
			printf("MenuButtonRelative X needs to move %i steps\n", X);
			if(X >= 0){
				RL->Move(Left, X);
			}else{
				RL->Move(Right, -X);
			}

		}
		void PressRightArrowButtonFromOrigin(){
			FB->Move(Forward, 6500);
			RL->Move(Left, 5000);
			UD->Move(Down, 1200);
		}
		void PressRightArrowButtonRelative(){
			Y = 6500 - FB->StepLoc;
			printf("RightArrowButtonRelative Y needs to move %i steps\n", Y);
			if(Y >= 0){
				FB->Move(Forward, Y);
			}else{
				FB->Move(Backward, -Y);
			}
			X = 5000 - RL->StepLoc;
			printf("RightArrowButtonRelative X needs to move %i steps\n", X);
			if(X >= 0){
				RL->Move(Left, X);
			}else{
				RL->Move(Right, -X);
			}
		}
		void PressLeftArrowButtonFromOrigin(){
			FB->Move(Forward, 6500);
			RL->Move(Left, 9000);
			UD->Move(Down, 1200);
		}
		void PressLeftArrowButtonRelative(){
			Y = 6500 - FB->StepLoc;
			printf("LeftArrowButtonRelative Y needs to move %i steps\n", Y);
			if(Y >= 0){
				FB->Move(Forward, Y);
			}else{
				FB->Move(Backward, -Y);
			}
			X = 9000 - RL->StepLoc;
			printf("LeftArrowButtonRelative X needs to move %i steps\n", X);
			if(X >= 0){
				RL->Move(Left, X);
			}else{
				RL->Move(Right, -X);
			}
		}
		void PressMinusButtonFromOrigin(){
			FB->Move(Forward, 8700);
			RL->Move(Left, 7000);
			UD->Move(Down, 1200);
		}
		void PressMinusButtonRelative(){
			Y = 8700 - FB->StepLoc;
			printf("MinusButtonRelative Y needs to move %i steps\n", Y);
			if(Y >= 0){
				FB->Move(Forward, Y);
			}else{
				FB->Move(Backward, -Y);
			}
			X = 7000 - RL->StepLoc;
			printf("MinusButtonRelative X needs to move %i steps\n", X);
			if(X >= 0){
				RL->Move(Left, X);
			}else{
				RL->Move(Right, -X);
			}
		}
};

int main(void){

	wiringPiSetup();

	Stepper RightLeft(0, 6, 2, 7, Right);
	Stepper ForwardBackward(3, 4, 5, 8, Backward);
	Servo UpDown(1, 9, Up);

	Controller myControl(&RightLeft, &ForwardBackward, &UpDown);

	printf("Resetting to Origin\n");
	myControl.ResetToOrigin();
	printf("ResetToOrigin Complete\n");
	delay(1000);

	printf("Now starting PressPlusButtonRelative\n");
	myControl.PressPlusButtonRelative();
	printf("PressPlusButtonRelative Finished\n");
	delay(1000);

	printf("Now Starting PressButton2Relative\n");
	myControl.PressButton2Relative();
	printf("PressButton2Relative finished\n");
	delay(1000);

	printf("Now Starting PressButton1Relative\n");
	myControl.PressButton1Relative();
	printf("PressButton1Relative finished\n");
	delay(1000);

	printf("Starting Left Arrow\n");
	myControl.PressLeftArrowButtonRelative();
	printf("success Left Arrow\n");
	delay(1000);

	printf("Starting Menu\n");
	myControl.PressMenuButtonRelative();
	printf("success Menu\n");
	delay(1000);

	printf("Starting Right Arrow\n");
	myControl.PressRightArrowButtonRelative();
	printf("success Right Arrow\n");
	delay(1000);

	printf("Starting Minus\n");
	myControl.PressMinusButtonRelative();
	printf("success Minus\n");
	delay(1000);

}
