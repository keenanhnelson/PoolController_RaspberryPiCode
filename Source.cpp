#include <stdio.h>
#include <iostream>
#include <string>
#include <unistd.h>
#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <wiringPi.h>
#include "opencv2/opencv.hpp"
#include <thread>
#include <mutex>

using boost::asio::ip::tcp;
using namespace cv;
using namespace std;

class Stepper{
	int pinActivate, pinSignal, pinDirection, pinOrigin, OriginDirection, StepDelay;
	int PosDir;
	public:
	int StepLoc;
	Stepper(int pActivate, int pSignal, int pDirection, int pOrigin, int OriginDir,
				int InitialStepDelay){
		pinActivate = pActivate;
		pinSignal = pSignal;
		pinDirection = pDirection;
		pinOrigin = pOrigin;
		OriginDirection = OriginDir;
		PosDir = !OriginDir;
		StepDelay = InitialStepDelay;
		StepLoc = 0;
		pinMode(pinSignal, OUTPUT); 
		pinMode(pinActivate, OUTPUT);
		pinMode(pinDirection, OUTPUT);
		//digitalWrite(pinActivate, LOW);
	}
	void SetMoveSpeed(int SpeedToChangeTo){
		StepDelay = SpeedToChangeTo;
	}
	int GetMoveSpeed(){
		return(StepDelay);
	}
	void Move(int Direction, int Steps){
		digitalWrite(pinDirection, Direction);//direction 
		digitalWrite(pinActivate, HIGH);//turn on driver
		printf("waiting for stepper motor driver to turn on.\n");
		delay(100);//wait for driver to fully turn on 
		printf("done waiting\n");


		printf("Direction = %i\n", Direction);
		printf("PosDir = %i\n", PosDir);
		for(int i=0; i<Steps; i++){
			digitalWrite(pinSignal, HIGH);
			delayMicroseconds(StepDelay);//1000
			digitalWrite(pinSignal, LOW);
			delayMicroseconds(StepDelay);//1000
			if(Direction == PosDir){
				StepLoc++;
			}else{
				StepLoc--;
			}
		}
		printf("Step Absolute Location = %i\n", StepLoc);
		//digitalWrite(pinActivate, LOW);//put driver to sleep 
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
			delayMicroseconds(StepDelay);
			digitalWrite(pinSignal, LOW);
			delayMicroseconds(StepDelay);
			count++;
			StepLoc = 0;
		}
		printf("How many steps it took to get to origin = %i\n", count);
		//digitalWrite(pinActivate, LOW);//put driver to sleep 
	}
	void TurnOff(){
		digitalWrite(pinActivate, LOW);//put driver to sleep 
	}
	void TurnOnStepperToStabilizePosition(){
		digitalWrite(pinActivate, HIGH);//Wake up driver. Used to stabilize position
		//delayMicroseconds(1000000);//Sleep for 1ms to stabilize position
		printf("stabilizing position\n");
		delay(200);
		printf("stabilizing finished\n");
	}
};

class ServoPosition{
	int pinSignal;
	public:
	static const int Right=0, Left=1, Backward=0, Forward=1, Up=1, Down=0;
	ServoPosition(int pSignal){
		pinSignal = pSignal;
		//pwmWrite(pinSignal, 90);
		pinMode(pinSignal, PWM_OUTPUT);
		pwmSetMode(PWM_MODE_MS);
		pwmSetClock(384); //clock at 50kHz (20us tick)
		pwmSetRange(1000); //range at 1000 ticks (20ms)
	}

	void Move(int Position, int Duration){
		pwmWrite(pinSignal, Position);
		//pinMode(pinSignal, PWM_OUTPUT);
		//pwmSetMode(PWM_MODE_MS);
		//pwmSetClock(384); //clock at 50kHz (20us tick)
		//pwmSetRange(1000); //range at 1000 ticks (20ms)
		delay(Duration);
		//pinMode(pinSignal, INPUT);//Disable PWM
	}

	void MoveToOrigin(){
		pwmWrite(pinSignal, 50);
		//pinMode(pinSignal, PWM_OUTPUT);
		//pwmSetMode(PWM_MODE_MS);
		//pwmSetClock(384); //clock at 50kHz (20us tick)
		//pwmSetRange(1000); //range at 1000 ticks (20ms)
		delay(500);
		//pinMode(pinSignal, INPUT);//Disable PWM
	}

	void LockPosition(){
		pwmWrite(pinSignal, 75);//75
		delay(500);
	}
};

class ServoContinuous{
	int pinSignal, pinOrigin, OriginDirection;
	public:
	static const int Right=0, Left=1, Backward=0, Forward=1, Up=1, Down=0;
	ServoContinuous(int pSignal, int pOrigin, int OriginDir){
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
	int XButtonLoc[8] = 	{370  ,245    ,125   ,240   ,248   ,316    ,175   ,266};
	int YButtonLoc[8] = 	{4	  ,4	  ,4	 ,34	,56	   ,47     ,63    ,77 };
	int UpDownServo[8] = 	{87   ,87	  ,87    ,86    ,86	   ,83	   ,85	  ,83 };
	public:
		static const int Right=0, Left=1, Backward=1, Forward=0, Up=1, Down=0;
		int FirstB=0,SecondB=1,ThirdB=2,PlusB=3,MenuB=4,RightB=5,LeftB=6,MinusB=7;
		Stepper *RL, *FB;
		ServoPosition *UD;
		thread EjectButtonThread;
		mutex MotorControlMutex;

		Controller(Stepper *inputRL, Stepper *inputFB, ServoPosition *inputUD, 
					int inputEjectButtonPin){
			RL = inputRL;
			FB = inputFB;
			UD = inputUD;
			EjectButtonThread = thread(&Controller::HandleEjectStatus, this,
										inputEjectButtonPin);
			EjectButtonThread.detach();
			
		}
		void HandleEjectStatus(int EjectButtonPin){//Thread use only
			//Setup eject button
			pinMode(EjectButtonPin, INPUT);
			pullUpDnControl(EjectButtonPin, PUD_UP);
			bool LockPositionState = 0;

			while(1){
				if(digitalRead(EjectButtonPin) == 0 && LockPositionState == 0){
					MotorControlMutex.lock();
					MoveToUnlockPosition();
					LockPositionState = 1;
					TurnOffSteppers();
					MotorControlMutex.unlock();
				}
				if(digitalRead(EjectButtonPin) == 0 && LockPositionState == 1){
					MotorControlMutex.lock();
					MoveToLockPosition();
					LockPositionState = 0;
					TurnOffSteppers();
					MotorControlMutex.unlock();
				}
			}
		}
		void ResetToOrigin(){
			UD->MoveToOrigin();
			FB->MoveToOrigin();
			RL->MoveToOrigin();
		}
		void MoveToLockPosition(){
			ResetToOrigin();
			FB->Move(Controller::Backward, 12);
			UD->LockPosition();
		}
		void MoveToUnlockPosition(){
			UD->MoveToOrigin();
			//ResetToOrigin();
			//FB->Move(Controller::Backward, 220);
		}
		void TurnOffSteppers(){
			RL->TurnOff();
			FB->TurnOff();
		}
		void TurnOnSteppersToStabilizePosition(){
			RL->TurnOnStepperToStabilizePosition();
			FB->TurnOnStepperToStabilizePosition();
		}
		void PressButtonFromOrigin(int i){
			MotorControlMutex.lock();
			TurnOnSteppersToStabilizePosition();
			printf("Returning to Origin\n");
			ResetToOrigin();
			printf("Going to press button %s\n", ButtonName[i]);
			FB->Move(Backward, YButtonLoc[i]);
			RL->Move(Right, XButtonLoc[i]);
			UD->Move(UpDownServo[i], 700);
			UD->MoveToOrigin();
			printf("Button %s has been pressed\n", ButtonName[i]);
			MotorControlMutex.unlock();
		}
		void PressButtonRelative(int i){
			MotorControlMutex.lock();
			UD->MoveToOrigin();//Make sure servo is all the way up
			printf("Button %s has been pressed\n", ButtonName[i]);
			printf("Relatively going to press Button %s\n", ButtonName[i]);
			Y = YButtonLoc[i] - FB->StepLoc;
			printf("Button1Relative Y needs to move %i steps\n", Y);
			if(Y >= 0){
				FB->Move(Backward, Y);
			}else{
				FB->Move(Forward, -Y);
			}
			X = XButtonLoc[i] - RL->StepLoc;
			printf("Button1Relative X needs to move %i steps\n", X);
			if(X >= 0){
				RL->Move(Right, X);
			}else{
				RL->Move(Left, -X);
			}
			printf("Finished Relative %s\n", ButtonName[i]);
			MotorControlMutex.unlock();
		}
};

class Server{
	private:
		Controller *myControl;
		boost::asio::io_service myIO_service;
		boost::asio::ip::tcp::acceptor *myAcceptor;
		int myPort, pinLED1, pinLED2;
		int NumOfButtons = 9;
		char ButtonName[9][20]= {"One\n", "Two\n", "Three\n", "Plus\n" ,"Menu\n" ,"Right\n", "Left\n", "Minus\n", "TakeImage\n"};
	public:
		std::string myFilename = "ToSendDog.bmp";

		Server(Controller *inputControl, int port, int pLED1, int pLED2){
			myPort = port;
			myAcceptor = new boost::asio::ip::tcp::acceptor(
					myIO_service, 
					boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v6(), myPort));
			myControl = inputControl;
			pinLED1 = pLED1;
			pinLED2 = pLED2;
			pinMode(pinLED1, OUTPUT); 
			pinMode(pinLED2, OUTPUT); 
		}

		void Start(){
			try{
				boost::array<char, 1024> buf;
				size_t file_size = 0;

				std::cout << "Opening Webcam\n";
				VideoCapture cap(0);
				while(!cap.open(0)){
					printf("Waitng for Webcam to open...\n");
					delay(200);
				}
				Mat frame;
				cap >> frame;

				while(1){
					boost::asio::ip::tcp::socket mySocket(myIO_service);
					std::cout << "Waiting for client connection\n";
					myAcceptor->accept(mySocket);
					std::cout << "obtained client connection" << std::endl;

					boost::array<char, 128> buf;
					boost::system::error_code error;
					int Reading = 0;
					std::string ReceivedString;
					while(Reading == 0){
						size_t len = mySocket.read_some(boost::asio::buffer(buf), error);
						std::cout.write(buf.data(), len);
						for(int i=0; i<len; i++){
							ReceivedString += buf.data()[i];
							if(buf.data()[i] == '\n'){
								Reading = 1;
							}
						}
					}
					if(ReceivedString == ButtonName[8]){//Take Image
						digitalWrite(pinLED1, HIGH);//Turn on LED
						digitalWrite(pinLED2, HIGH);//Turn on LED
						delay(2000);
						//clear out image buffer and take picture
						for(int b=0; b<5; b++){
							cap >> frame;
							//flip(frame, frame, -1);
						}
						digitalWrite(pinLED1, LOW);//Turn off LED
						digitalWrite(pinLED2, LOW);//Turn off LED
						std::string TakePicFilename = "TakePic.png";
						imwrite(TakePicFilename, frame);
						FileTransfer(&mySocket, TakePicFilename);
					}
					else{
						for(int i=0; i<NumOfButtons; i++){
							if((ReceivedString == ButtonName[i])){
								string str = ReceivedString;
								str.erase(str.end()-1, str.end());

								std::string ImageFilename = "Before.png";
								digitalWrite(pinLED1, HIGH);//Turn on LED
								digitalWrite(pinLED2, HIGH);//Turn on LED
								//clear out image buffer and take picture
								for(int b=0; b<5; b++){
									cap >> frame;
									//flip(frame, frame, -1);
								}
								digitalWrite(pinLED1, LOW);//Turn off LED
								digitalWrite(pinLED2, LOW);//Turn off LED
								imwrite(ImageFilename, frame);
								FileTransfer(&mySocket, ImageFilename);

								myControl->PressButtonFromOrigin(i);

								ImageFilename = "After.png";
								digitalWrite(pinLED1, HIGH);//Turn on LED
								digitalWrite(pinLED2, HIGH);//Turn on LED
								//clear out image buffer and take picture
								for(int b=0; b<5; b++){
									cap >> frame;
									//flip(frame, frame, -1);
								}
								digitalWrite(pinLED1, LOW);//Turn off LED
								digitalWrite(pinLED2, LOW);//Turn off LED
								imwrite(ImageFilename, frame);
								FileTransfer(&mySocket, ImageFilename);

								myControl->MoveToLockPosition();
								myControl->TurnOffSteppers();
							}
						}
					}

					//Read confirmation message from client
					boost::asio::streambuf ReceiveBuf;
				    boost::asio::read_until(mySocket, ReceiveBuf, "\n");
				    std::istream ReceiveStream(&ReceiveBuf);
				    std::string ClientConfirm;            
				    ReceiveStream >> ClientConfirm;
					std::cout << "ClientConfirm = " << ClientConfirm << std::endl;

					//std::cout << "Message Received: " << ReceivedString;
					std::string message = "";
					message = "Button pressed: " + ReceivedString;
					std::cout << message << std::endl;

					boost::system::error_code ignored_error;
					boost::asio::write(mySocket, boost::asio::buffer(message), ignored_error);
				}
			}
			catch(std::exception& e){
				std::cerr << e.what() << std::endl;
			}
		}

		void FileTransfer(boost::asio::ip::tcp::socket *myFileSocket,
				std::string myFilename){
			boost::array<char, 1024> buf;
			size_t file_size = 0;
			try
			{
				boost::system::error_code myFileError;
				std::ifstream source_file(myFilename, std::ios_base::binary | std::ios_base::ate);
				if (!source_file)
				{
					std::cout << "failed to open " << myFilename << std::endl;
				}
				size_t file_size = source_file.tellg();
				source_file.seekg(0);
				// first send file name and file size to server
				boost::asio::streambuf request;
				std::ostream request_stream(&request);
				request_stream << myFilename << "\n"
					<< file_size << "\n\n";
				boost::asio::write(*myFileSocket, request);
				std::cout << "start sending file content.\n";
				for (;;)
				{

					if (source_file.eof()==false)
					{
						//place file into array
						source_file.read(buf.c_array(), (std::streamsize)buf.size());
						if (source_file.gcount()<=0)
						{
							std::cout << "read file error " << std::endl;
						}
						boost::asio::write(*myFileSocket, 
							boost::asio::buffer(buf.c_array(), source_file.gcount()),
							boost::asio::transfer_all(),
							myFileError);
						if (myFileError)
						{
							std::cout << "send myFileError:" << myFileError << std::endl;
						}
					}
					else{
						break;
					}
				}
			}
			catch (std::exception& e)
			{
				std::cerr << e.what() << std::endl;
			}
		}
};

int main(int a, char **b){

	wiringPiSetup();

	Stepper ForwardBackward(0, 6, 2, 7, Controller::Forward, 8000);//5000 3000 8000
	Stepper RightLeft(3, 4, 5, 8, Controller::Left, 1100);//1000
	ServoPosition UpDown(1);


	Controller myControl(&RightLeft, &ForwardBackward, &UpDown, 13);
	Server myServer(&myControl, 24957, 12, 14);
	myServer.Start();

	////Let user decide what button to press
	//Controller myControl(&RightLeft, &ForwardBackward, &UpDown, 13);
	//while(1){
	//	cout << "Enter desire button number\n";
	//	int ButtonNumber;
	//	cin >> ButtonNumber;
	//	if( !((ButtonNumber >= 0) && (ButtonNumber <= 7)) ){
	//		break;
	//	}
	//	myControl.PressButtonFromOrigin(ButtonNumber);
	//}
	//myControl.TurnOffSteppers();


	return(0);
}
