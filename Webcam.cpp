#include <iostream>
#include "Webcam.hpp"
#include <wiringPi.h>

//Initializes pins for controlling the LEDs
Webcam::Webcam(int pLED1, int pLED2){
	pinLED1 = pLED1;
	pinLED2 = pLED2;
	pinMode(pinLED1, OUTPUT); 
	pinMode(pinLED2, OUTPUT); 
}

Webcam::~Webcam(){
	delete cap;
}

void Webcam::OpenCam(){
	std::cout << "Opening Webcam\n";
	cap = new cv::VideoCapture(0);
	while(!cap->open(0)){
		printf("Waiting for Webcam to open...\n");
		delay(200);
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
	digitalWrite(pinLED1, HIGH);//Turn on LED
	digitalWrite(pinLED2, LOW);//Turn off LED
	delay(2000);//Make sure camera has enough time to self calibrate
	*OutImg1 = TakePic();
	
	digitalWrite(pinLED1, LOW);//Turn off LED
	digitalWrite(pinLED2, HIGH);//Turn on LED
	*OutImg2 = TakePic();
	
	//Turn off LEDs
	digitalWrite(pinLED1, LOW);//Turn off LED
	digitalWrite(pinLED2, LOW);//Turn off LED
}

//Decide which led will be on when picture is taken
cv::Mat Webcam::TakePicWithLED(bool LED1State, bool LED2State){
	digitalWrite(pinLED1, LED1State);//Turn on/off LED
	digitalWrite(pinLED2, LED2State);//Turn on/off LED
	delay(2000);
	
	cv::Mat Frame = TakePic();
	
	digitalWrite(pinLED1, LOW);//Turn off LED
	digitalWrite(pinLED2, LOW);//Turn off LED
	return(Frame);
}

//Copys non-glare from one image to the glare part of another
cv::Mat Webcam::RemoveLED_Glare(cv::Mat ImgInput1, cv::Mat ImgInput2){
	cv::Rect TopLeft(160,120,70,70);
	cv::Rect TopRight(340,130,70,70);
	cv::Rect BotRight(330,260,70,70);
	cv::Rect BotLeft(155,250,70,70);
	
	cv::Mat ImgTopLeft = ImgInput2(TopLeft);
	cv::Mat ImgTopRight = ImgInput2(TopRight);
	cv::Mat ImgBotRight = ImgInput2(BotRight);
	cv::Mat ImgBotLeft = ImgInput2(BotLeft);
	
	ImgTopLeft.copyTo(ImgInput1(TopLeft));
	ImgTopRight.copyTo(ImgInput1(TopRight));
	ImgBotRight.copyTo(ImgInput1(BotRight));
	ImgBotLeft.copyTo(ImgInput1(BotLeft));
	
	return(ImgInput1);
}

//clear out image buffer and take picture
cv::Mat Webcam::TakePic(){
	cv::Mat Frame;
	for(int b=0; b<5; b++){
		(*cap) >> Frame;
	}
	return(Frame);
}
