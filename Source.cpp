#include <stdio.h>
#include <iostream>
#include <string>
#include <unistd.h>
#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <wiringPi.h>
#include "opencv2/opencv.hpp"

using boost::asio::ip::tcp;
using namespace cv;
using namespace std;

//void CaptureImage(string name, VideoCapture *cap);
void CaptureImage(string name);

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
	int UpDownServo[8] = 	{1300 ,1300   ,1400  ,1300  ,1300  ,1300   ,1300  ,1300};
	//int UpDownServo[8] = 	{1400 ,1400   ,1400  ,1400  ,1200  ,1200   ,1200  ,1200};
	public:
		static const int Right=0, Left=1, Backward=1, Forward=0, Up=1, Down=0;
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

class Server{
	private:
		Controller *myControl;
		boost::asio::io_service myIO_service;
		boost::asio::ip::tcp::acceptor *myAcceptor;
		int myPort;
		int NumOfButtons = 9;
		char ButtonName[9][20]= {"One\n", "Two\n", "Three\n", "Plus\n" ,"Menu\n" ,"Right\n", "Left\n", "Minus\n", "TakeImage\n"};
	public:
		std::string myFilename = "ToSendDog.bmp";

		Server(Controller *inputControl, int port){
			myPort = port;
			myAcceptor = new boost::asio::ip::tcp::acceptor(
					myIO_service, 
					boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v6(), myPort));
			myControl = inputControl;
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
						//clear out image buffer and take picture
						for(int b=0; b<5; b++){
							cap >> frame;
						}
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
								//CaptureImage(ImageFilename);
								//clear out image buffer and take picture
								for(int b=0; b<5; b++){
									cap >> frame;
								}
								imwrite(ImageFilename, frame);
								FileTransfer(&mySocket, ImageFilename);

								myControl->PressButtonFromOrigin(i);
								sleep(1);

								ImageFilename = "After.png";
								//CaptureImage(ImageFilename);
								//clear out image buffer and take picture
								for(int b=0; b<5; b++){
									cap >> frame;
								}
								imwrite(ImageFilename, frame);
								FileTransfer(&mySocket, ImageFilename);

								myControl->ResetToOrigin();
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

	Stepper ForwardBackward(0, 6, 2, 7, Controller::Forward, 10000);
	Stepper RightLeft(3, 4, 5, 8, Controller::Left, 1000);
	//Servo UpDown(1, 9, Controller::Up);
	//Controller myControl(&RightLeft, &ForwardBackward, &UpDown);
	//int NumOfButtons = 8;
	//char ButtonName[8][20]= {"One\n", "Two\n", "Three\n", "Plus\n" ,"Menu\n" ,"Right\n", "Left\n", "Minus\n"};

	//Server myServer(&myControl, 24957);

	//myServer.Start();


	//if(a==5){
		RightLeft.MoveToOrigin();//Direction Steps
		ForwardBackward.MoveToOrigin();//Direction Steps
		//RightLeft.Move(atoi(b[1]), atoi(b[2]));//Direction Steps
		//ForwardBackward.Move(atoi(b[3]), atoi(b[4]));//Direction Steps
	//}else{
	//	printf("incorrect number of arguments\n");
	//}

	cout << "Type anything and hit enter to turn off motors\n";
	int wait;
	cin >> wait;
	RightLeft.TurnOff();
	ForwardBackward.TurnOff();

	return(0);

	//Controller myControl(&RightLeft, &ForwardBackward, &UpDown);

	//myControl.PressButtonFromOrigin(myControl.SecondB);
	//myControl.ResetToOrigin();
	//myControl.PressButtonRelative(myControl.MenuB);
	//myControl.PressButtonRelative(myControl.FirstB);
	//for(int i=0; i<8; i++){
	//	myControl.PressButtonRelative(i);
	//}
}
//void CaptureImage(string name, VideoCapture *cap){
void CaptureImage(string name){
	VideoCapture cap(0);
	while(!cap.open(0)){
		printf("Waitng for Webcam to open...\n");
		delay(200);
	}
	Mat frame;
	cap >> frame;
	imwrite(name, frame);
}
