#include "Server.hpp"
#include <wiringPi.h>
#include <fstream>

Server::Server(Controller *inputControl, Webcam *inputCam, int port){
	myPort = port;
	myAcceptor = new boost::asio::ip::tcp::acceptor(
			myIO_service, 
			boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v6(), myPort));
	myControl = inputControl;
	Cam = inputCam;
}

void Server::Start(){
	try{
		Cam->OpenCam();

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
				for(size_t i=0; i<len; i++){
					ReceivedString += buf.data()[i];
					if(buf.data()[i] == '\n'){
						Reading = 1;
					}
				}
			}
			if(ReceivedString == ButtonName[TakeImageB]){//Take Image
				Cam->GetRemoteScreenImg(TakePicFilename);
//				Cam->TakePic(TakePicFilename);
				FileTransfer(&mySocket, TakePicFilename);
			}
			else{
				for(int i=0; i<NumOfButtons; i++){
					if((ReceivedString == ButtonName[i])){
						//Take before pic
						Cam->GetRemoteScreenImg(BeforePicFilename);
//						Cam->TakePic(BeforePicFilename);
						FileTransfer(&mySocket, BeforePicFilename);

						//Press the desired button
						myControl->PressButtonFromOrigin(i);

						//Take after pic
						Cam->GetRemoteScreenImg(AfterPicFilename);
//						Cam->TakePic(AfterPicFilename);
						FileTransfer(&mySocket, AfterPicFilename);

						//Lock and turn off motors
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

void Server::FileTransfer(boost::asio::ip::tcp::socket *myFileSocket,
		std::string myFilename){
	boost::array<char, 1024> buf;
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
