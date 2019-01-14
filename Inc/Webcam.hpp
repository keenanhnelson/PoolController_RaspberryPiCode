#ifndef WEBCAM_HPP
#define WEBCAM_HPP

#include "opencv2/opencv.hpp"

class Webcam{
	public:
		Webcam(int pLED1, int pLED2);
		~Webcam();
		void OpenCam();
		//void TakePic(std::string PicFilename);
		void GetRemoteScreenImg(std::string PicFilename);
		void LED_State(bool LED1State, bool LED2State);
	private:
		cv::VideoCapture *cap;
		int pinLED1, pinLED2;
		
		void TakePicsWithAlternateLEDs(cv::Mat *OutImg1, cv::Mat *OutImg2);
		cv::Mat TakePicWithLED(bool LED1State, bool LED2State);
		cv::Mat RemoveLED_Glare(cv::Mat ImgInput1, cv::Mat ImgInput2);
		cv::Mat TakePic();
};

#endif //WEBCAM_HPP