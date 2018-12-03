#ifndef STEPPER_HPP
#define STEPPER_HPP

class Stepper{
	int pinActivate, pinSignal, pinDirection, pinOrigin, OriginDirection, StepDelay;
	int PosDir;
	public:
		int StepLoc;
		Stepper(int pActivate, int pSignal, int pDirection, int pOrigin, int OriginDir,
					int InitialStepDelay);
		void SetMoveSpeed(int SpeedToChangeTo);
		int GetMoveSpeed();
		void Move(int Direction, int Steps);
		void MoveToOrigin();
		void TurnOff();
		void TurnOnStepperToStabilizePosition();
};
#endif //STEPPER_HPP