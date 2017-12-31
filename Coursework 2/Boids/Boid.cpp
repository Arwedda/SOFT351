#include "DXUT.h"
#include "Boid.h"

std::mt19937 rng;
std::uniform_int_distribution<uint32_t> turn(1, 3);
std::uniform_int_distribution<uint32_t> acceleration(1, 3);


Boid::Boid()
	:Thing3D() {
	speed = 0.0;
	maxForward = 10.0;
	maxReverse = 0.0;
	dragCoefficient = 0.5;		//Playing with arbitrary numbers
	mass = 300.0;				//Google says bears generally are a few hundred kilos
	maxTilt = 0.52;
	maxClimb = 0.79;
	maxDescent = 1.55;
	wingRest = -0.44;
	wingPosition = -0.44;
	rng.seed(std::time(0));
}

Boid::Boid(float setX, float setY, float setZ)
	:Thing3D(setX, setY, setZ) {
	speed = 0.0;
	maxForward = 10.0;
	maxReverse = 0.0;
	dragCoefficient = 0.5;		//Playing with arbitrary numbers
	mass = 300.0;				//Google says bears generally are a few hundred kilos
	maxTilt = 0.52;
	maxClimb = 0.79;
	maxDescent = 1.55;
	wingRest = -0.44;
	wingPosition = -0.44;
	rng.seed(std::time(0));
}

Boid::~Boid() {
}

bool Boid::isNear(Boid* flockMember, float range) {
	XMVECTOR dist = distance(flockMember);

	return (XMVectorGetX(dist) + XMVectorGetY(dist) + XMVectorGetZ(dist) <= range);
}

bool Boid::isNear(float bearX, float bearY, float bearZ, float range) {
	float deltaX = abs(long (getX() - bearX));
	float deltaY = abs(long (getY() - bearY));
	float deltaZ = abs(long (getZ() - bearZ));

	return (deltaX + deltaY + deltaZ <= range);
}

XMVECTOR Boid::distance(Boid* flockMember) {
	float deltaX = abs(long(getX() - flockMember->getX()));
	float deltaY = abs(long(getY() - flockMember->getY()));
	float deltaZ = abs(long(getZ() - flockMember->getZ()));

	XMVECTOR dist = XMVectorSet(deltaX, deltaY, deltaZ, 0.0);

	return dist;
}

void Boid::follow(float fElapsedTime, bool tooClose) {
	if (-maxForward < speed && !tooClose) {
		speed -= fElapsedTime * 3;
	} else {
		speed += fElapsedTime * 3;
	}
	move(fElapsedTime);
}

void Boid::move(float fElapsedTime) {
	/* //Quaternion rotation - sort of works, not quite right sometimes
	XMVECTOR xAxis = XMVectorSet(0, 1, 0, 0);
	XMVECTOR yAxis = XMVectorSet(1, 0, 0, 0);
	XMVECTOR zAxis = XMVectorSet(0, 0, 1, 0);

	XMVECTOR xRotation = XMQuaternionRotationAxis(xAxis, getRX());
	XMVECTOR yRotation = XMQuaternionRotationAxis(yAxis, getRY());
	XMVECTOR zRotation = XMQuaternionRotationAxis(zAxis, getRZ());

	XMVECTOR combinedRotation = XMQuaternionMultiply(XMQuaternionMultiply(yRotation, zRotation), xRotation);
	matRotations = XMMatrixRotationQuaternion(combinedRotation); */

	//Calculate current direction
	matRotations = XMMatrixRotationRollPitchYaw(getRY(), getRX(), getRZ());
	currentDir = XMVector3TransformCoord(getInitialDirection(), matRotations);
	currentDir = XMVector3Normalize(currentDir);

	//Move bear in that direction by the speed
	currentDir *= getSpeed() * fElapsedTime;

	setX(getX() + XMVectorGetX(currentDir));
	setY(getY() + XMVectorGetY(currentDir));
	setZ(getZ() + XMVectorGetZ(currentDir));
}

void Boid::faceBear(XMVECTOR bearDir, float fElapsedTime) {


}

//Separation: steer to avoid crowding local flockmates 
void Boid::separation(std::vector<Boid*> flock, float minProximity) {
	XMVECTOR forward = XMVectorSet(getRX(), getRY(), getRZ(), 0.0);
	XMVECTOR avoidance = XMVectorSet(0.0, 0.0, 0.0, 0.0);
	Boid nearest;

	while (!flock.empty()) {
		if (isNear(flock.back(), minProximity)) {
			nearest = *flock.back();
			avoidance -= (forward - XMVectorSet(nearest.getX(), nearest.getY(), nearest.getZ(), 0.0));
		}
		flock.pop_back();
	}

	avoidance = XMVector3Normalize(avoidance);
	forward = XMVector3Normalize(forward);

	//Calculate angle between my Curr direction and nearest flockmate
	XMVECTOR angleBetween = XMVector3AngleBetweenNormals(forward, avoidance);
	//Turn away from close flockmates
	setRX(getRX() + (XMVectorGetX(angleBetween) / 1000.0));
}

//Alignment: steer towards the average heading of local flockmates and match speeds
void Boid::alignment(std::vector<Boid*> flock) {
	int flockSize = flock.size();
	//Prevent divide by 0
	if (0 < flockSize) {
		float targetRX = 0.0;
		//float targetRY = 0.0;
		float targetSpeed = 0.0;

		while (!flock.empty()) {
			//Make total RX, RY
			targetRX += flock.back()->getRX();
			//targetRY += flock.back()->getRY();
			targetSpeed -= (getSpeed() - flock.back()->getSpeed());

			flock.pop_back();
		}
		//Divide each by flock size
		targetRX = targetRX / flockSize;
		//targetRY /= flockSize;
		targetSpeed = targetSpeed / flockSize;

		//Turn 0.1% towards this position
		setRX(getRX() + (targetRX / 1000.0));
		//setRY(getRY() + (targetRY / 1000.0));
		setSpeed(getSpeed() + (targetSpeed / 1000.0));
	}
}

//Cohesion: steer to move toward the average position of local flockmates
void Boid::cohesion(std::vector<Boid*> flock) {
	XMVECTOR forward = XMVectorSet(getRX(), getRY(), getRZ(), 0.0);
	XMVECTOR targetPosition;
	int flockSize = flock.size();
	float avgX = 0.0;
	float avgY = 0.0;
	float avgZ = 0.0;

	while (!flock.empty()) {
		avgX += flock.back()->getX();
		avgY += flock.back()->getY();
		avgZ += flock.back()->getZ();

		flock.pop_back();
	}
	//Divide by size to create average position
	avgX = avgX / flockSize;
	avgY = avgY / flockSize;
	avgZ = avgZ / flockSize;

	targetPosition = XMVectorSet(avgX, avgY, avgZ, 0.0);

	targetPosition = XMVector3Normalize(targetPosition);
	forward = XMVector3Normalize(forward);

	XMVECTOR angleBetween = XMVector3AngleBetweenNormals(forward, targetPosition);
	
	//Turn 0.01% towards this position
	setRX(getRX() + (XMVectorGetX(angleBetween) / 100000.0));
}

void Boid::moveRandomly(float fElapsedTime) {
	turnRandomly(fElapsedTime);
	adjustSpeed(fElapsedTime);
}

void Boid::turnRandomly(float fElapsedTime) {
	int decision = turn(rng);
	switch (decision) {
	case 1:
		turnLeft(fElapsedTime);
		break;
	case 2:
		turnRight(fElapsedTime);
		break;
		//case 3: forward
	}
}

//Random speed adjustments that encourages boids to be moving
//at approximately half speed
void Boid::adjustSpeed(float fElapsedTime) {
	int decision = acceleration(rng);
	//Below half speed
	if (getSpeed() >= (-getMaxForward() / 2)) {
		switch (decision) {
			case 1:
			case 2:
				forward(fElapsedTime);
				break;
			case 3:
				reverse(fElapsedTime);
				break;
		}
	} else { //Above half speed 
		switch (decision) {
			case 1:
				forward(fElapsedTime);
				break;
			case 2:
			case 3:
				reverse(fElapsedTime);
				break;
		}
	}
}

void Boid::turnLeft(float fElapsedTime) {
	setRX(getRX() - fElapsedTime * 3);
}

void Boid::turnRight(float fElapsedTime) {
	setRX(getRX() + fElapsedTime * 3);
}

void Boid::forward(float fElapsedTime) {
	if (speed > -maxForward) {
		speed -= fElapsedTime * 3;
	}
}

void Boid::reverse(float fElapsedTime) {
	if (speed < maxReverse) {
		speed += fElapsedTime * 3;
	}
}





void Boid::tiltLeft(float fElapsedTime) {
	if (getRZ() < maxTilt) {
		setRZ(getRZ() + fElapsedTime * 3);
	}
}

void Boid::tiltRight(float fElapsedTime) {
	if (getRZ() > -maxTilt) {
		setRZ(getRZ() - fElapsedTime * 3);
	}
}

void Boid::straightenUp(float fElapsedTime, float horizontalRZ) {
	if (getRZ() < horizontalRZ) {
		setRZ(getRZ() + fElapsedTime * 3);
	}
	else if (getRZ() > horizontalRZ) {
		setRZ(getRZ() - fElapsedTime * 3);
	}
}





float Boid::getSpeed() {
	return speed;
}

void Boid::setSpeed(float newSpeed) {
	speed = newSpeed;
}

float Boid::getFallSpeed() {
	return fallSpeed;
}
void Boid::setFallSpeed(float newFallSpeed) {
	fallSpeed = newFallSpeed;
}

float Boid::getMaxForward() {
	return maxForward;
}

float Boid::getMaxReverse() {
	return maxReverse;
}

float Boid::getMaxTilt() {
	return maxTilt;
}

float Boid::getMaxClimb() {
	return maxClimb;
}

float Boid::getMaxDescent() {
	return maxDescent;
}

float Boid::getWingRest() {
	return wingRest;
}

float Boid::getWingPosition() {
	return wingPosition;
}

void Boid::setWingPosition(float newWingPosition) {
	wingPosition = newWingPosition;
}

void Boid::slowDown(float fElapsedTime, float airDensity) {
	float speed = getSpeed();
	float drag = -0.5f * airDensity * speed * abs((long)speed) * dragCoefficient;
	setSpeed(speed + drag / mass * fElapsedTime);
}

void Boid::slowGround() {
	float speed = getSpeed();
	setSpeed(speed * 0.99);
}

void Boid::fall(float gravityFallSpeed) {
	setFallSpeed(getFallSpeed() + gravityFallSpeed);
	float newHeight = getY() - getFallSpeed();
	if (newHeight >= 0.0) {
		setY(newHeight);
	}
	else {
		setSpeed(0.0);
		setFallSpeed(0.0);
		setY(0.0);
	}
}

void Boid::wingFlap() {
	wingPosition = sin(timeGetTime() / 200.0);
}

void Boid::restWings() {
	if (wingPosition > wingRest + 0.01 || wingPosition < wingRest - 0.01) {
		wingFlap();
	}
}

void Boid::tiltUp(float fElapsedTime) {
	if (getRY() > -getMaxClimb()) {
		setRY(getRY() - fElapsedTime * 3);
	}
}

void Boid::tiltDown(float fElapsedTime) {
	if (getRY() < getMaxDescent()) {
		setRY(getRY() + fElapsedTime * 3);
	}
}

void Boid::levelOut(float fElapsedTime, float horizontalRY) {
	if (getRY() > horizontalRY) {
		tiltUp(fElapsedTime);
		setSpeed(0);
	}
}

bool Boid::inAir(float ground) {
	return (getY() > ground);
}