#ifndef CONFIG_HPP
#define CONFIG_HPP

//Forward backward stepper control
#define FB_ActivatePin 0
#define FB_SignalPin 6
#define FB_DirectionPin 2
#define FB_EndStopPin 7
#define FB_StepPeriod 8000 //5000 3000 8000

//Right left stepper control
#define RL_ActivatePin 3
#define RL_SignalPin 4
#define RL_DirectionPin 5
#define RL_EndStopPin 8
#define RL_StepPeriod 1100

//Up down servo control
#define UpDn_SignalPin 1

//Controller class
#define EjectRemotePin 13

//Server class
#define LED1Pin 12
#define LED2Pin 14 
#define ServerPort 24957

#endif //CONFIG_HPP