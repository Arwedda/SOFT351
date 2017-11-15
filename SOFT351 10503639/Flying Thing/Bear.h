#include "Thing3D.h"
#ifndef BearH   //Guards, as usual.
#define BearH

//**************************************************************************//
// Many of these member variables are public and have to be set by writing  //
// to the variable.   OO programmers would have us write many more          //
// setThisandThat(...) methods.                                             //
//**************************************************************************//

class Bear : public Thing3D
{
private:
	float		speed, fallSpeed;				//Speeds
	float		maxForward, maxReverse;			//Max speeds
	float		dragCoefficient, mass;			//Variables for slowing down
	float		maxTilt;						//Tilt whilst turning
	float		maxClimb, maxDescent;			//Manual climb/descend rate
	float		wingRest, wingPosition;			//Wing angle
public:
	XMVECTOR vecRear;
	Bear();
	~Bear();
	float getSpeed();
	void setSpeed(float newSpeed);
	float getFallSpeed();
	void setFallSpeed(float newFallSpeed);
	float getMaxForward();
	float getMaxReverse();
	float getMaxTilt();
	float getMaxClimb();
	float getMaxDescent();
	float getWingRest();
	float getWingPosition();
	void setWingPosition(float newWingPosition);

	void turnLeft(float fElapsedTime);
	void tiltLeft(float fElapsedTime);
	void turnRight(float fElapsedTime);
	void tiltRight(float fElapsedTime);
	void tiltUp(float fElapsedTime);
	void tiltDown(float fElapsedTime);
	void forward(float fElapsedTime);
	void reverse(float fElapsedTime);
	void slowDown(float fElapsedTime, float airDensity);
	void slowGround();
	void fall(float fElapsedTime);
	void wingFlap();
	void restWings();
	void straightenUp(float fElapsedTime, float horizontalRZ);
	void levelOut(float fElapsedTime, float horizontalRY);
	bool inAir(float ground);
	void roar();
	void move(float fElapsedTime);
};
#endif
