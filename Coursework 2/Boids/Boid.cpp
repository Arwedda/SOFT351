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

Boid::Boid(float startX, float startY, float startZ, float startRX, float startRY, float startRZ)
	:Thing3D(startX, startY, startZ, startRX, startRY, startRZ) {
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
	float dist = distance(XMVectorSet(flockMember->getX(), flockMember->getY(), flockMember->getZ(), 0.0));

	return (dist <= range);
}

bool Boid::isNear(XMVECTOR xyzPos, float range) {
	float dist = distance(xyzPos);

	return (dist <= range);
}

float Boid::distance(XMVECTOR xyzPos) {
	float deltaX = abs(long(getX() - XMVectorGetX(xyzPos)));
	float deltaY = abs(long(getY() - XMVectorGetY(xyzPos)));
	float deltaZ = abs(long(getZ() - XMVectorGetZ(xyzPos)));

	return (deltaX + deltaY + deltaZ);
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

	matRotations = XMMatrixRotationRollPitchYaw(getRY(), getRX(), getRZ());
	currentDir = XMVector3TransformCoord(getInitialDirection(), matRotations);
	currentDir = XMVector3Normalize(currentDir);

	//Apply speed to turn it into a vector
	currentDir *= getSpeed() * fElapsedTime;

	setX(getX() + XMVectorGetX(currentDir));
	setY(getY() + XMVectorGetY(currentDir));
	setZ(getZ() + XMVectorGetZ(currentDir));
}

void Boid::fleeBear(XMVECTOR bearPos, float fElapsedTime) {
	XMVECTOR position = XMVectorSet(getX(), getY(), getZ(), 0.0);
	XMVECTOR movementVector = createMovementVector(getRX(), getRY(), getRZ(), fElapsedTime);
	XMVECTOR boidToTarget = position - bearPos;

	movementVector = XMVector3Normalize(movementVector);
	boidToTarget = XMVector3Normalize(boidToTarget);

	XMVECTOR angleBetween = XMVector3AngleBetweenNormals(movementVector, boidToTarget);

	/*
	//If we are closer to our target continue, otherwise invert the turn direction		--- Does not work :(
	float proposedNewDirection = getRX() + (XMVectorGetX(angleBetween) / 40);
	if (!turnedTowardsTarget(proposedNewDirection, angleBetween, boidToTarget, fElapsedTime)) {
	angleBetween *= -1;
	}
	*/

	//Turn 5% clockwise towards the point
	setRX(getRX() + (XMVectorGetX(angleBetween) / 20.0));
}

//Separation: steer to avoid crowding local flockmates 
void Boid::separation(std::vector<Boid*> flock, float separationStrength, float minProximity, float fElapsedTime) {
	XMVECTOR position = XMVectorSet(getX(), getY(), getZ(), 0.0);
	XMVECTOR movementVector = createMovementVector(getRX(), getRY(), getRZ(), fElapsedTime);
	XMVECTOR boidToTarget;
	XMVECTOR avoidancePosition;
	XMVECTOR angleBetween;
	movementVector = XMVector3Normalize(movementVector);

	while (!flock.empty()) {
		if (isNear(flock.back(), minProximity)) {
			avoidancePosition = XMVectorSet(flock.back()->getX(), flock.back()->getY(), flock.back()->getZ(), 0.0);
			//The movement vector AWAY FROM the position to avoid
			boidToTarget = position - avoidancePosition;
			boidToTarget = XMVector3Normalize(boidToTarget);
			//Angle between currenct vector and position 180 degrees/PI radians from avoidance position
			angleBetween = XMVector3AngleBetweenNormals(movementVector, boidToTarget);
			 
			/*
			//If we are closer to our target continue, otherwise invert the turn direction		--- Does not work :(
			float proposedNewDirection = getRX() + (XMVectorGetX(angleBetween) / 40);
			if (!turnedTowardsTarget(proposedNewDirection, angleBetween, boidToTarget, fElapsedTime)) {
			angleBetween *= -1;
			}
			*/

			//Turn 2.5% clockwise towards the point
			setRX(getRX() + (separationStrength * XMVectorGetX(angleBetween) / 40.0));
		}
		flock.pop_back();
	}
}

//Alignment: steer towards the average heading of local flockmates
//To make 3d simply add RY values
void Boid::alignment(std::vector<Boid*> flock, float alignmentStrength) {
	int flockSize = flock.size();
	//Prevent divide by 0
	if (0 < flockSize) {
		float targetRX = 0.0;

		while (!flock.empty()) {
			//Make total RX
			targetRX += flock.back()->getRX();
			flock.pop_back();
		}
		//Divide by flock size
		targetRX = targetRX / flockSize;

		targetRX -= getRX();

		//Turn 1% towards the target direction
		setRX(getRX() + (alignmentStrength * targetRX / 100.0));
	}
}

//Cohesion: steer to move toward the average position of local flockmates
void Boid::cohesion(std::vector<Boid*> flock, float cohesionStrength, float fElapsedTime) {
	XMVECTOR position = XMVectorSet(getX(), getY(), getZ(), 0.0);
	XMVECTOR targetPosition;
	int flockSize = flock.size();
	float avgX = 0.0;
	float avgZ = 0.0;

	while (!flock.empty()) {
		avgX += flock.back()->getX();
		avgZ += flock.back()->getZ();

		flock.pop_back();
	}
	//Divide by size to create average position
	avgX = avgX / flockSize;
	avgZ = avgZ / flockSize;

	targetPosition = XMVectorSet(avgX, 0.0, avgZ, 0.0);

	//Angle between these = turn required to face average position
	XMVECTOR movementVector = createMovementVector(getRX(), getRY(), getRZ(), fElapsedTime);
	XMVECTOR boidToTarget = targetPosition - position;

	movementVector = XMVector3Normalize(movementVector);
	boidToTarget = XMVector3Normalize(boidToTarget);

	XMVECTOR angleBetween = XMVector3AngleBetweenNormals(movementVector, boidToTarget);

	/*
	//If we are closer to our target continue, otherwise invert the turn direction		--- Does not work :(
	float proposedNewDirection = getRX() + (XMVectorGetX(angleBetween) / 40);
	if (!turnedTowardsTarget(proposedNewDirection, angleBetween, boidToTarget, fElapsedTime)) {
	angleBetween *= -1;
	}
	*/


	//Turn 0.1% clockwise towards the point
	setRX(getRX() + (cohesionStrength * XMVectorGetX(angleBetween) / 1000.0));
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

//If boids get too far from the centre of a circle (around the static camera) then they are forced to turn back
//towards it
void Boid::leash(XMVECTOR leashPosition, float leashStrength, float leashLength, float fElapsedTime) {
	if (!isNear(leashPosition, leashLength)) {
		XMVECTOR position = XMVectorSet(getX(), getY(), getZ(), 0.0);

		//Angle between these = turn required to face leash
		XMVECTOR movementVector = createMovementVector(getRY(), getRX(), getRZ(), fElapsedTime);
		XMVECTOR boidToLeash = leashPosition - position;

		movementVector = XMVector3Normalize(movementVector);
		boidToLeash = XMVector3Normalize(boidToLeash);

		XMVECTOR angleBetween = XMVector3AngleBetweenNormals(boidToLeash, movementVector);
	
		/*
		//If we are closer to our target continue, otherwise invert the turn direction		--- Does not work :(
		float proposedNewDirection = getRX() + (XMVectorGetX(angleBetween) / 40);
		if (!turnedTowardsTarget(proposedNewDirection, angleBetween, boidToLeash, fElapsedTime)) {
			angleBetween *= -1;
		}
		*/

		//Turn 2.5% clockwise towards the point
		setRX(getRX() + (leashStrength * XMVectorGetX(angleBetween) / 40.0));
	}
}

//Creates the movement vector before cohesion, separation and leashing so that a turning angle can be calculated
XMVECTOR Boid::createMovementVector(float xRot, float yRot, float zRot, float fElapsedTime) {
	//Calculate current direction
	XMMATRIX locRotations = XMMatrixRotationRollPitchYaw(yRot, xRot, zRot);
	XMVECTOR locDir = XMVector3TransformCoord(getInitialDirection(), locRotations);
	locDir = XMVector3Normalize(locDir);

	//Apply speed to turn it into a vector
	locDir *= getSpeed() * fElapsedTime;

	//Return the movement vector
	return locDir;
}

//Calculates whether we have picked the correct turn direction
bool Boid::turnedTowardsTarget(float proposedRX, XMVECTOR oldAngleBetween, XMVECTOR positionToTarget, float fElapsedTime) {
	//Create a movement vector based on the proposed turn
	XMVECTOR proposedMovementVector = createMovementVector(proposedRX, getRY(), getRZ(), fElapsedTime);
	proposedMovementVector = XMVector3Normalize(proposedMovementVector);

	//Calculates the angle between the target, the boid's position and a step towards proposedRX + RX (if close to 0, the target itself)
	XMVECTOR angleBetween = XMVector3AngleBetweenNormals(positionToTarget, proposedMovementVector);

	return (XMVectorGetX(angleBetween) < XMVectorGetX(oldAngleBetween));
}

float Boid::getSpeed() {
	return speed;
}

float Boid::getMaxForward() {
	return maxForward;
}