#ifndef SERVER_HPP
#define SERVER_HPP

#include <boost/array.hpp>
#include <boost/asio.hpp>
#include "Controller.hpp"
#include "Webcam.hpp"

class Server{
	private:
		Controller *myControl;
		Webcam *Cam;
		boost::asio::io_service myIO_service;
		boost::asio::ip::tcp::acceptor *myAcceptor;
		int myPort;
		//Make sure NumOfButtons, Button_e, and ButtonName follow one another
		const int NumOfButtons = 9;
		typedef enum{FirstB, SecondB, ThirdB, PlusB, MenuB, RightB, LeftB, MinusB, TakeImageB}Button_e;
		const char ButtonName[9][20]= {
			"One\n", "Two\n", "Three\n", "Plus\n" ,"Menu\n",
			"Right\n", "Left\n", "Minus\n", "TakeImage\n",
		};
		const std::string ImgLoc = "/home/pi/JeffPoolController/Main/Img/";
		const std::string TakePicFilename = ImgLoc + "TakePic.png";
		const std::string BeforePicFilename = ImgLoc + "Before.png";
		const std::string AfterPicFilename = ImgLoc + "After.png";
		void FileTransfer(boost::asio::ip::tcp::socket *myFileSocket,
							std::string myFilename);
	public:
		Server(Controller *inputControl, Webcam *inputCam, int port);
		void Start();

};
#endif //SERVER_HPP