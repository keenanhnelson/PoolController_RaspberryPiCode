#include <iostream>
#include "Webcam.hpp"
#include "pigpio.h"

#define CAM_CAL_WAIT 1500 //The amount of time (in ms) to wait for the camera to self calibrated after the LEDs are turned on

//Initializes pins for controlling the LEDs
Webcam::Webcam(int pLED1, int pLED2){
	pinLED1 = pLED1;
	pinLED2 = pLED2;
	gpioSetMode(pinLED1, PI_OUTPUT); 
	gpioSetMode(pinLED2, PI_OUTPUT); 
}

Webcam::~Webcam(){
	delete cap;
}

void Webcam::OpenCam(){
	std::cout << "Opening Webcam\n";
	cap = new cv::VideoCapture(0);
	while(!cap->open(0)){
		printf("Waiting for Webcam to open...\n");
		gpioDelay(200*1000);
	}
}

//Grabs current image of pool remote screen
void Webcam::GetRemoteScreenImg(std::string PicFilename){
	cv::Mat Img1, Img2;
	TakePicsWithAlternateLEDs(&Img1, &Img2);
	
	cv::Mat Output = RemoveLED_Glare(Img1, Img2);
	imwrite(PicFilename, Output);
}

//Take two pictures in quick succession while alternating LEDs
void Webcam::TakePicsWithAlternateLEDs(cv::Mat *OutImg1, cv::Mat *OutImg2){
	LED_State(1, 0);
	gpioDelay(CAM_CAL_WAIT*1000);//Make sure camera has enough time to self calibrate
	*OutImg1 = TakePic();
	
	LED_State(0, 1);
	*OutImg2 = TakePic();
	
	//Turn off LEDs
	LED_State(0, 0);
}

//Decide which led will be on when picture is taken
cv::Mat Webcam::TakePicWithLED(bool LED1State, bool LED2State){
	LED_State(LED1State, LED2State);
	gpioDelay(CAM_CAL_WAIT*1000);
	
	cv::Mat Frame = TakePic();
	
	LED_State(0, 0);
	return(Frame);
}

//Copys non-glare from one image to the glare part of another
cv::Mat Webcam::RemoveLED_Glare(cv::Mat ImgInput1, cv::Mat ImgInput2){
	//LED set that lights up in a diamond shape
	cv::Rect Top(230,60,70,70);
	cv::Rect Right(400,160,70,70);
	cv::Rect Bot(240,270,70,70);
	cv::Rect Left(50,160,70,70);
	
	cv::Mat ImgTop = ImgInput1(Top);
	cv::Mat ImgRight = ImgInput1(Right);
	cv::Mat ImgBot = ImgInput1(Bot);
	cv::Mat ImgLeft = ImgInput1(Left);
	
	ImgTop.copyTo(ImgInput2(Top));
	ImgRight.copyTo(ImgInput2(Right));
	ImgBot.copyTo(ImgInput2(Bot));
	ImgLeft.copyTo(ImgInput2(Left));
	
	return(ImgInput2);
}

//clear out image buffer and take picture
cv::Mat Webcam::TakePic(){
	cv::Mat Frame;
	for(int b=0; b<5; b++){
		(*cap) >> Frame;
	}
	return(Frame);
}

void Webcam::LED_State(bool LED1State, bool LED2State){
	gpioWrite(pinLED1, LED1State);//1 turns led on. 0 turns led off
	gpioWrite(pinLED2, LED2State);//1 turns led on. 0 turns led off
}