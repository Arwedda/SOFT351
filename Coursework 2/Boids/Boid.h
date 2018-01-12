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
	Boid(float setX, float setY, float setZ, float startRX, float startRY, float startRZ);
	~Boid();
	bool isNear(Boid* flockMember, float range);
	bool isNear(XMVECTOR xyzPos, float range);
	float distance(XMVECTOR xyzPos);
	void move(float fElapsedTime);
	void fleeBear(XMVECTOR bearPos, float fElapsedTime);
	void separation(std::vector<Boid*> flock, float separationStrength, float minProximity, float fElapsedTime);
	void alignment(std::vector<Boid*> flock, float alignmentStrength);
	void cohesion(std::vector<Boid*> flock, float cohesionStrength, float fElapsedTime);
	void moveRandomly(float fElapsedTime);
	void turnRandomly(float fElapsedTime);
	void adjustSpeed(float fElapsedTime);
	void turnLeft(float fElapsedTime);
	void turnRight(float fElapsedTime);
	void forward(float fElapsedTime);
	void reverse(float fElapsedTime);
	void leash(XMVECTOR leashPosition, float leashStrength, float leashLength, float fElapsedTime);
	XMVECTOR createMovementVector(float xRot, float yRot, float zRot, float fElapsedTime);
	bool turnedTowardsTarget(float proposedRX, XMVECTOR oldAngleBetween, XMVECTOR positionToTarget, float fElapsedTime);
	float getSpeed();
	float getMaxForward();
};
#endif
