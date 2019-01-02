#ifndef SERVO_POSITION_HPP
#define SERVO_POSITION_HPP

class ServoPosition{
	int pinSignal;
	public:
		static const int Right=0, Left=1, Backward=0, Forward=1, Up=1, Down=0;
		ServoPosition(int pSignal);
		void Move(int Position, int Duration);
		void MoveToOrigin();
		void LockPosition();
};
#endif //SERVO_POSITION_HPP