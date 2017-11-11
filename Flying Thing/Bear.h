#include "DXUT.h"
#include "DXUTmisc.h"
#include "SDKmisc.h"
#include "SDKmisc.h"
#include "SDKMesh.h"
#include <xnamath.h>

#ifndef BearH   //Guards, as usual.
#define BearH

//**************************************************************************//
// Many of these member variables are public and have to be set by writing  //
// to the variable.   OO programmers would have us write many more          //
// setThisandThat(...) methods.                                             //
//**************************************************************************//

class Bear
{
private:
	float x, y, z;							//Position.
	float rx, ry, rz;						//Rotation (direction)
	float speed, maxForward, maxReverse;	//Speeds
	float sx, sy, sz;						//Scales in x, y, z directions.
											//float frameTime;						//Used to calculate elapsed times
	float		maxTilt;					//Tilt whilst turning
	float		maxClimb, maxDescent;		//Manual climb/descend rate
	float		wingRest, wingPosition;		//Wing angle
	XMVECTOR initialDirection;				//Initial direction
public:
	XMVECTOR  currentDir;					//Current direction
	XMMATRIX  matRotations;
	XMMATRIX  matScale;
	XMMATRIX  matTranslate;

	XMMATRIX  matWorld;			//We need to pass the world view and
	XMMATRIX  matView;			//projection matrices to the snader.
	XMMATRIX  matProjection;
	XMMATRIX  matWorldViewProjection;

	UINT      textureNumber;
												//**********************************************************************/
												// This is where encapsulation falls down some.  The shader file has its//
												// lighting attributes.  This doesn't really make sense surely light is //
												// a globl thing, not an attribute of an object?  It makes partial sense//
												// here, as I am assuming that in a simple world. all objects will share//
												// the same shader.														//
												//**********************************************************************//
/*	XMVECTOR		vecLightDirection;				//w value unused.
	XMFLOAT4		lightDiffuseColour;				//w value unused.
	XMFLOAT4		LightAmbientColour;				//w value unused.
													//**********************************************************************/
													// Public methods.														//
													//**********************************************************************//
public:
	Bear();
	float getX();
	void setX(float newX);
	float getY();
	void setY(float newX);
	float getZ();
	void setZ(float newX);
	float getRX();
	void setRX(float newX);
	float getRY();
	void setRY(float newX);
	float getRZ();
	void setRZ(float newX);
	float getSpeed();
	void setSpeed(float newSpeed);
	float getMaxForward();
	float getMaxReverse();
	float getSX();
	float getSY();
	float getSZ();
	float getMaxTilt();
	float getMaxClimb();
	float getMaxDescent();
	float getWingRest();
	float getWingPosition();
	void setWingPosition(float newWingPosition);
	XMVECTOR getInitialDirection();

	void turnLeft(float fElapsedTime);
	void tiltLeft(float fElapsedTime);
	void turnRight(float fElapsedTime);
	void tiltRight(float fElapsedTime);
	void tiltUp(float fElapsedTime);
	void tiltDown(float fElapsedTime);
	void forward(float fElapsedTime);
	void reverse(float fElapsedTime);
	void slowDown(float fElapsedTime);
	void fall(float fElapsedTime);
	void wingFlap();
	void restWings();
	void straightenUp(float fElapsedTime, float horizontalRZ);
	void levelOut(float fElapsedTime, float horizontalRY);
	bool inAir(float ground);
	void roar();
	XMVECTOR move(float fElapsedTime);
};
#endif
