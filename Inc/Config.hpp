#ifndef CONFIG_HPP
#define CONFIG_HPP

//Forward backward stepper control
#define FB_ActivatePin 17
#define FB_SignalPin 25
#define FB_DirectionPin 27
#define FB_EndStopPin 4
#define FB_StepPeriod 8000 //5000 3000 8000

//Right left stepper control
#define RL_ActivatePin 22
#define RL_SignalPin 23
#define RL_DirectionPin 24
#define RL_EndStopPin 2
#define RL_StepPeriod 1400

//Up down servo control
#define UpDn_SignalPin 18

//Controller class
#define EjectRemotePin 9

//Webcam class
#define LED1Pin 10
#define LED2Pin 11 

//Server class
#define ServerPort 24957

#endif //CONFIG_HPP