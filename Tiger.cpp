#include "DXUT.h"
#include "DXUTcamera.h"
#include "DXUTgui.h"
#include "DXUTsettingsDlg.h"
#include "SDKmisc.h"
#include "SDKMesh.h"
#include <xnamath.h>
#include "resource.h"
#include "Tiger.h"

void turnLeft(float fElapsedTime);
void tiltLeft(float fElapsedTime);
void turnRight(float fElapsedTime);
void tiltRight(float fElapsedTime);
bool isNotTurning();
void straightenUp(float fElapsedTime);
void tiltUp(float fElapsedTime);
void tiltDown(float fElapsedTime);
void levelOut(float fElapsedTime);
void forward(float fElapsedTime);
void reverse(float fElapsedTime);
void slowDown(float fElapsedTime);
void fall(float fElapsedTime);
void wingFlap();
void restWings();
bool tigerInAir();
void roar();

struct POSITION {
	float		X = 0;
	float		Y = 0;
	float		Z = 2;
};

struct DIRECTION {
	float		RX = 0;
	float		RY = 0;
	float		RZ = 0;
};

POSITION	*position;
DIRECTION	*direction;

float		speed = 0;
float		maxSpeed = 0;
float		maxReverse = 0;
float		maxTilt = 0;
float		maxClimb = 0;
float		maxDescent = 0;
float		wingRest = 0;
float		wingPosition = 0;
XMVECTOR    initDir;

float		horizontalRY = 0.0;
float		horizontalRZ = 0.0;

Tiger::Tiger()
{
	maxSpeed = 10;
	maxReverse = -3;
	maxTilt = 0.52;
	maxClimb = 0.79;
	maxDescent = -1.55;
	wingRest = -0.44;
	wingPosition = -0.44;
	initDir = XMVectorSet(0, 0, -2, 0);
}

Tiger::~Tiger()
{
}

void turnLeft(float fElapsedTime) {
	direction->RX -= fElapsedTime * 3;
}

void tiltLeft(float fElapsedTime) {
	if (direction->RZ > -maxTilt) {
		direction->RZ -= fElapsedTime * 3;
	}
}

void turnRight(float fElapsedTime) {
	direction->RX += fElapsedTime * 3;
}

void tiltRight(float fElapsedTime) {
	if (direction->RZ < maxTilt) {
		direction->RZ += fElapsedTime * 3;
	}
}

void straightenUp(float fElapsedTime) {
	if (direction->RZ < horizontalRZ) {
		direction->RZ += fElapsedTime * 3;
	}
	else if (direction->RZ > horizontalRZ) {
		direction->RZ -= fElapsedTime * 3;
	}
}

void tiltUp(float fElapsedTime) {
	if (direction->RY < maxClimb) {
		direction->RY += fElapsedTime * 3;
	}
}

void tiltDown(float fElapsedTime) {
	if (direction->RY > maxDescent) {
		direction->RY -= fElapsedTime * 3;
	}
}

void levelOut(float fElapsedTime) {
	if (direction->RY < horizontalRY) {
		tiltUp(fElapsedTime);
		speed = 0;
	}
}

void forward(float fElapsedTime) {
	if (speed < maxSpeed) {
		speed += fElapsedTime * 3;
	}
}

void reverse(float fElapsedTime) {
	if (speed > maxReverse) {
		speed -= fElapsedTime * 3;
	}
}

void slowDown(float fElapsedTime) {
	if (speed > 0) {
		speed -= fElapsedTime * 3;
	}
	else if (speed < 0) {
		speed += fElapsedTime * 3;
	}
}

void fall(float fElapsedTime) {
	position->Y -= fElapsedTime * 3;
}

void wingFlap() {
	wingPosition = sin(timeGetTime() / 200.0);
	PlaySound(L"Media\\Wing\\flap.wav", NULL, SND_ASYNC | SND_NOSTOP);
}

void restWings() {
	if (wingPosition > wingRest + 0.01 || wingPosition < wingRest - 0.01) {
		wingFlap();
	}
}

bool tigerInAir() {
	return (position->Y > 0);
}

void roar() {
	PlaySound(L"Media\\Tiger\\roar.wav", NULL, SND_ASYNC | SND_NOSTOP);
}