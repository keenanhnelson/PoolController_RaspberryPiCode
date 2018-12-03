#ifndef SERVER_HPP
#define SERVER_HPP

#include <boost/array.hpp>
#include <boost/asio.hpp>
#include "opencv2/opencv.hpp"
#include "Controller.hpp"

class Server{
	private:
		Controller *myControl;
		boost::asio::io_service myIO_service;
		boost::asio::ip::tcp::acceptor *myAcceptor;
		int myPort, pinLED1, pinLED2;
		int NumOfButtons = 9;
		char ButtonName[9][20]= {
			"One\n", "Two\n", "Three\n", "Plus\n" ,"Menu\n",
			"Right\n", "Left\n", "Minus\n", "TakeImage\n",
		};
		
		void FileTransfer(boost::asio::ip::tcp::socket *myFileSocket,
							std::string myFilename);
	public:
		Server(Controller *inputControl, int port, int pLED1, int pLED2);
		void Start();

};
#endif //SERVER_HPP