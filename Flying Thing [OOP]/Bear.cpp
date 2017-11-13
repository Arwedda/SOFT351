#include "Bear.h"

Bear::Bear()
	:Thing3D() {
	speed = 0.0;
	maxForward = 10.0;
	maxReverse = 3.0;
	dragCoefficient = 1000.0;	//Playing with arbitrary numbers
	mass = 300.0;				//Brown bear (heaviest) are up to 600kg
	maxTilt = 0.52;
	maxClimb = 0.79;
	maxDescent = 1.55;
	wingRest = -0.44;
	wingPosition = -0.44;
}

Bear::~Bear() {
}

float Bear::getSpeed() {
	return speed;
}

void Bear::setSpeed(float newSpeed) {
	speed = newSpeed;
}

float Bear::getFallSpeed() {
	return fallSpeed;
}
void Bear::setFallSpeed(float newFallSpeed) {
	fallSpeed = newFallSpeed;
}

float Bear::getMaxForward() {
	return maxForward;
}

float Bear::getMaxReverse() {
	return maxReverse;
}

float Bear::getMaxTilt() {
	return maxTilt;
}

float Bear::getMaxClimb() {
	return maxClimb;
}

float Bear::getMaxDescent() {
	return maxDescent;
}

float Bear::getWingRest() {
	return wingRest;
}

float Bear::getWingPosition() {
	return wingPosition;
}

void Bear::setWingPosition(float newWingPosition) {
	wingPosition = newWingPosition;
}

void Bear::turnLeft(float fElapsedTime) {
	setRX(getRX() - fElapsedTime * 3);
}

void Bear::tiltLeft(float fElapsedTime) {
	if (getRZ() < maxTilt) {
		setRZ(getRZ() + fElapsedTime * 3);
	}
}

void Bear::turnRight(float fElapsedTime) {
	setRX(getRX() + fElapsedTime * 3);
}

void Bear::tiltRight(float fElapsedTime) {
	if (getRZ() > -maxTilt) {
		setRZ(getRZ() - fElapsedTime * 3);
	}
}

void Bear::forward(float fElapsedTime) {
	if (speed > -maxForward) {
		speed -= fElapsedTime * 3;
	}
}

void Bear::reverse(float fElapsedTime) {
	if (speed < maxReverse) {
		speed += fElapsedTime * 3;
	}
}

void Bear::slowDown(float fElapsedTime, float airDensity) {
	float speed = getSpeed();
	float drag = -0.5f * airDensity * speed * abs((long)speed) * dragCoefficient;
	setSpeed(speed + drag / mass * fElapsedTime);
}

void Bear::slowGround() {
	float speed = getSpeed();
	setSpeed(speed * 0.999);
}

void Bear::fall(float gravityFallSpeed) {
	setFallSpeed(getFallSpeed() + gravityFallSpeed);
	float newHeight = getY() - getFallSpeed();
	if (newHeight >= 0.0) {
		setY(newHeight);
	}
	else {
		setY(0.0);
	}
}

void Bear::wingFlap() {
	wingPosition = sin(timeGetTime() / 200.0);
	PlaySound(L"Media\\Bear\\flap.wav", NULL, SND_ASYNC | SND_NOSTOP);
}

void Bear::restWings() {
	if (wingPosition > wingRest + 0.01 || wingPosition < wingRest - 0.01) {
		wingFlap();
	}
}

void Bear::straightenUp(float fElapsedTime, float horizontalRZ) {
	if (getRZ() < horizontalRZ) {
		setRZ(getRZ() + fElapsedTime * 3);
	}
	else if (getRZ() > horizontalRZ) {
		setRZ(getRZ() - fElapsedTime * 3);
	}
}

void Bear::tiltUp(float fElapsedTime) {
	if (getRY() > -getMaxClimb()) {
		setRY(getRY() - fElapsedTime * 3);
	}
}

void Bear::tiltDown(float fElapsedTime) {
	if (getRY() < getMaxDescent()) {
		setRY(getRY() + fElapsedTime * 3);
	}
}

void Bear::levelOut(float fElapsedTime, float horizontalRY) {
	if (getRY() > horizontalRY) {
		tiltUp(fElapsedTime);
		setSpeed(0);
	}
}

bool Bear::inAir(float ground) {
	return (getY() > ground);
}

void Bear::roar() {
	PlaySound(L"Media\\Bear\\roar.wav", NULL, SND_ASYNC | SND_NOSTOP);
}

XMVECTOR Bear::move(float fElapsedTime) {
	/* Quaternion rotation
	Tried with just x and y axis - still
	Currently inverts rotate about Y-axis and causes barrel rolls after a half turn

	XMVECTOR xAxis = XMVectorSet(1, 0, 0, 0);
	XMVECTOR yAxis = XMVectorSet(0, 1, 0, 0);
	XMVECTOR zAxis = XMVectorSet(0, 0, 1, 0);

	XMVECTOR xRotation = XMQuaternionRotationAxis(xAxis, bear->RY);
	XMVECTOR yRotation = XMQuaternionRotationAxis(yAxis, bear->RX);
	XMVECTOR zRotation = XMQuaternionRotationAxis(zAxis, bear->RZ);

	XMVECTOR combinedRotation = XMQuaternionMultiply(XMQuaternionMultiply(zRotation, yRotation), xRotation);
	XMMATRIX matRotation = XMMatrixRotationQuaternion(combinedRotation);
	*/

	//Calculate current direction
	matRotations = XMMatrixRotationRollPitchYaw(getRY(), getRX(), getRZ());
	currentDir = XMVector3TransformCoord(getInitialDirection(), matRotations);
	currentDir = XMVector3Normalize(currentDir);
	XMVECTOR vecRear = currentDir * -3;

	//Move bear in that direction by the speed
	currentDir *= getSpeed() * fElapsedTime;

	setX(getX() + XMVectorGetX(currentDir));
	setY(getY() + XMVectorGetY(currentDir));
	setZ(getZ() + XMVectorGetZ(currentDir));

	return vecRear;
}
