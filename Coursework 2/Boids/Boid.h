#include "Thing3D.h"
#include <vector>
#include <ctime>
#include <random>
#ifndef BoidH   //Guards, as usual.
#define BoidH
class Boid : public Thing3D
{
private:
	float		speed, fallSpeed;				//Speeds
	float		maxForward, maxReverse;			//Max speeds
	float		dragCoefficient, mass;			//Variables for slowing down
	float		maxTilt;						//Tilt whilst turning
	float		maxClimb, maxDescent;			//Manual climb/descend rate
	float		wingRest, wingPosition;			//Wing angle

public:
	Boid();
	Boid(float setX, float setY, float setZ);
	~Boid();
	bool isNear(Boid* flockMember, float range);
	bool isNear(XMVECTOR xyzPos, float range);
	XMVECTOR distance(Boid* flockMember);
	XMVECTOR distance(XMVECTOR xyzPos);
	void move(float fElapsedTime);
	void faceBear(XMVECTOR bearDir, float fElapsedTime);
	void separation(std::vector<Boid*> flock, float minProximity);
	void alignment(std::vector<Boid*> flock);
	void cohesion(std::vector<Boid*> flock);
	void moveRandomly(float fElapsedTime);
	void turnRandomly(float fElapsedTime);
	void adjustSpeed(float fElapsedTime);
	void turnLeft(float fElapsedTime);
	void turnRight(float fElapsedTime);
	void forward(float fElapsedTime);
	void reverse(float fElapsedTime);
	void leash(XMVECTOR leashPosition, float leashLength);


	void tiltLeft(float fElapsedTime);
	void tiltRight(float fElapsedTime);
	void straightenUp(float fElapsedTime, float horizontalRZ);



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
	void tiltUp(float fElapsedTime);
	void tiltDown(float fElapsedTime);
	void slowDown(float fElapsedTime, float airDensity);
	void slowGround();
	void fall(float fElapsedTime);
	void wingFlap();
	void restWings();
	void levelOut(float fElapsedTime, float horizontalRY);
	bool inAir(float ground);
};
#endif
