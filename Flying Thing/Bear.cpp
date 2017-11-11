#include "Bear.h"
#include <xnamath.h>

Bear::Bear() {
	x = 0.0;
	y = 0.0;
	z = 2.0;
	rx = -2;
	ry = 0.0;
	rz = 0.0;
	speed = 0.0;
	maxForward = 10.0;
	maxReverse = 3.0;
	sx = 10.0;
	sy = 10.0;
	sz = 10.0;
	//frameTime = 0;
	maxTilt = 0.52;
	maxClimb = 0.79;
	maxDescent = 1.55;
	wingRest = -0.44;
	wingPosition = -0.44;
	initialDirection = XMVectorSet(-2, 0, 0, 0);
	//currentDir = initialDir;
}

float Bear::getX() {
	return x;
}

void Bear::setX(float newX) {
	x = newX;
}

float Bear::getY() {
	return y;
}

void Bear::setY(float newY) {
	y = newY;
}

float Bear::getZ() {
	return z;
}

void Bear::setZ(float newZ) {
	z = newZ;
}

float Bear::getRX() {
	return rx;
}

void Bear::setRX(float newRX) {
	rx = newRX;
}

float Bear::getRY() {
	return ry;
}

void Bear::setRY(float newRY) {
	ry = newRY;
}

float Bear::getRZ() {
	return rz;
}

void Bear::setRZ(float newRZ) {
	rz = newRZ;
}

float Bear::getSpeed() {
	return speed;
}

void Bear::setSpeed(float newSpeed) {
	speed = newSpeed;
}

float Bear::getMaxForward() {
	return maxForward;
}

float Bear::getMaxReverse() {
	return maxReverse;
}

float Bear::getSX() {
	return sx;
}

float Bear::getSY() {
	return sy;
}

float Bear::getSZ() {
	return sz;
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

XMVECTOR Bear::getInitialDirection() {
	return initialDirection;
}

void Bear::turnLeft(float fElapsedTime) {
	rx -= fElapsedTime * 3;
}

void Bear::tiltLeft(float fElapsedTime) {
	if (rz < maxTilt) {
		rz += fElapsedTime * 3;
	}
}

void Bear::turnRight(float fElapsedTime) {
	rx += fElapsedTime * 3;
}

void Bear::tiltRight(float fElapsedTime) {
	if (rz > -maxTilt) {
		rz -= fElapsedTime * 3;
	}
}

void Bear::tiltUp(float fElapsedTime) {
	if (ry > -maxClimb) {
		ry -= fElapsedTime * 3;
	}
}

void Bear::tiltDown(float fElapsedTime) {
	if (ry < maxDescent) {
		ry += fElapsedTime * 3;
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

void Bear::slowDown(float fElapsedTime) {
	if (speed > 0) {
		speed -= fElapsedTime * 3;
	}
	else if (speed < 0) {
		speed += fElapsedTime * 3;
	}
}

void Bear::fall(float fElapsedTime) {
	y -= fElapsedTime * 3;
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

void Bear::roar() {
	PlaySound(L"Media\\Bear\\roar.wav", NULL, SND_ASYNC | SND_NOSTOP);
}