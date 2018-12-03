#ifndef SERVO_CONTINUOUS_HPP
#define SERVO_CONTINUOUS_HPP

class ServoContinuous{
	int pinSignal, pinOrigin, OriginDirection;
	public:
	static const int Right=0, Left=1, Backward=0, Forward=1, Up=1, Down=0;
	ServoContinuous(int pSignal, int pOrigin, int OriginDir);
	void Move(int Direction, int Duration);
	void MoveToOrigin();
};
#endif //SERVO_CONTINUOUS_HPP