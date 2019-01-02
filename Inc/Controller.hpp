#ifndef CONTROLLER_HPP
#define CONTROLLER_HPP

#include "Stepper.hpp"
#include "ServoPosition.hpp"
#include <thread>
#include <mutex>

class Controller{
	private:
		int X, Y;
		//Button_e, ButtonName, XButtonLoc, YButtonLoc, and UpDownServo need to follow one another
		typedef enum{FirstB, SecondB, ThirdB, PlusB, MenuB, RightB, LeftB, MinusB}Button_e;
		char ButtonName[8][20]= {"1"  ,"2"    ,"3"   ,"Plus","Menu","Right","Left","Minus"};
		int XButtonLoc[8] = 	{370  ,245    ,125   ,240   ,248   ,316    ,175   ,266};
		int YButtonLoc[8] = 	{4	  ,4	  ,4	 ,34	,56	   ,47     ,63    ,77 };
		int UpDownServo[8] = 	{87   ,87	  ,87    ,86    ,86	   ,83	   ,85	  ,83 };
	public:
		static const int Right=0, Left=1, Backward=1, Forward=0, Up=1, Down=0;
		Stepper *RL, *FB;
		ServoPosition *UD;
		std::thread EjectButtonThread;
		std::mutex MotorControlMutex;

		Controller(Stepper *inputRL, Stepper *inputFB, ServoPosition *inputUD, 
					int inputEjectButtonPin);
		void HandleEjectStatus(int EjectButtonPin);//Thread use only
		void ResetToOrigin();
		void MoveToLockPosition();
		void MoveToUnlockPosition();
		void TurnOffSteppers();
		void TurnOnSteppersToStabilizePosition();
		void PressButtonFromOrigin(int i);
		void PressButtonRelative(int i);
};
#endif //CONTROLLER_HPP