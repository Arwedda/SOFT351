//**************************************************************************//
// Class to implement a bouncing ball.     It is not yet finished.  None of //
// Nigel's demos ever are :-(												//
//                                                                          //
// All a but messier than Java - you declare the members and prototypes for //
// the methods in the header file, but the implementation of the methods is //
// given in the "cpp" file.                                                 //


//**************************************************************************//
// This code is copyright of Dr Nigel Barlow, lecturer in computing,		//
// University of Plymouth, UK.  email: nigel@soc.plymouth.ac.uk.			//
//																			//
// You may use, modify and distribute this (rather cack-handed in places)	//
// code subject to the following conditions:								//
//																			//
//	1:	You may not use it, or sell it, or use it in any adapted form for	//
//		financial gain, without my written premission.						//
//																			//
//	2:	You must not remove the copyright messages.							//
//	                                                                        //
//	3:	You should learn to type and spell (read some of the speiong and	//
//		tyoing in the commebts.											    //
//**************************************************************************//

#include "Ball.h"



//******************************************************************//
// Constructor.  Force the parent constructor to execute, do our	//
// thing.															//
//******************************************************************//
Ball::Ball(ID3D11Device        *pRenderingDevice,
		   ID3D11DeviceContext *pImmediateContext)
	: Thing3D(pRenderingDevice, pImmediateContext)
{
	groundY         = 0;		
	bouncyness      = 0.8f;
	g		        = 9.8f;		// Derr, surely this should be global to the planet?
	r               = 0.2f;
	mass            = 1.45f;	//Is this a bit heavy?
	revPerMin       = 600.0f;	// revolution per minute, to calculate spin
	airDensity      = 1.2f;		// Air at 1 atm pressure, at around 20 degrees Celcius
	dragCoefficient = 0.47f;	// Drag coefficient of a spherical body

	xSpeed = ySpeed = zSpeed = 0;	//Initially still.
	}



//**************************************************************************//
// What?  Nigel caught writing accessor methods?							//
//**************************************************************************//
void Ball::setXYZspeed(float newXSpeed, float newYSpeed, float newZSpeed)
{
	xSpeed = newXSpeed; 	ySpeed = newYSpeed;    zSpeed = newZSpeed;
}


 
//**************************************************************************//
//																			//
// Make the ball bounce.	Code is in no way optimised.					//
//																			//
// We assume that there is no air resistance, and that the therefore the	//
// only acceleration is downwards due to gravity.							//
//																			//
//**************************************************************************//
void Ball::Bounce()
{
	float deltaXspeed = 0, deltaYspeed = 0, deltaZspeed = 0;
	
	
	float pi               = XMVectorGetX(g_XMPi);	//What a weird place to put PI.
	float spin             = revPerMin / (600.0f * 2.0f * pi);
 	float crossSectionArea = pi * (float)pow(r, 2);



	//**********************************************************************//
	// Change in ySpeed only every frame.  Change due to acceleration		//
	// downwards due to gravity.											//
	//**********************************************************************//
	deltaYspeed -= g*frameTime;	 
	ySpeed += deltaYspeed;	  


	//**********************************************************************//
	// Air resistance; slows the ball down.									//
	// I'm not using speed^2, I'm using speed * abs(xSpeed to keep the sign	//
	// positive or negative.												//
	//**********************************************************************//	
	float xDrag = -0.5f * airDensity * xSpeed * abs(xSpeed) * dragCoefficient * crossSectionArea;
	deltaXspeed += xDrag  / mass * frameTime;
	
	float yDrag = -0.5f * airDensity * ySpeed * abs(ySpeed) * dragCoefficient * crossSectionArea;
	deltaYspeed += yDrag / mass * frameTime;
  
	float zDrag = -0.5f * airDensity * zSpeed * abs(zSpeed) * dragCoefficient * crossSectionArea;
	deltaZspeed += zDrag  / mass * frameTime;
  
	


	//**********************************************************************//
	// Magnus effect; makes it bend.	We need to create a direction vector//
	// and rotate it by 90 degrees.  Assume the ball is only spinning about //
	// the y axis only.														//
	//																		//
	// Does the ball bend?  I don't think it does.	I think it speeds up as //
	// well :-(																//
	//**********************************************************************//
	float magnus = -0.5 * spin * abs(xSpeed) * xSpeed * airDensity * dragCoefficient * crossSectionArea;
    
	XMVECTOR vecDirection = XMVectorSet(xSpeed, ySpeed, zSpeed, 0);	//W unused.
	XMMATRIX matRotateY   = XMMatrixRotationY(XMConvertToRadians(90));
	XMVECTOR vecMagMagnus = XMVector3TransformCoord(vecDirection, matRotateY) * magnus;
	

	
	deltaXspeed += XMVectorGetX(vecMagMagnus) / mass * frameTime;
	deltaYspeed += XMVectorGetY(vecMagMagnus) / mass * frameTime;
	deltaZspeed += XMVectorGetZ(vecMagMagnus) / mass * frameTime;
   
	


	//**********************************************************************//
	// Now calculate the change in speed and change in distance every frame.//
	//**********************************************************************//
	xSpeed += deltaXspeed;
	ySpeed += deltaYspeed;			// Calculate new speed by adding 
	zSpeed += deltaZspeed;			// change in speed.
	

	
	float deltaX = xSpeed*frameTime;  //Calculate distance travelled in
	float deltaY = ySpeed*frameTime;  //x, y, z directions.
	float deltaZ = zSpeed*frameTime;


	x += deltaX;	
	y += deltaY;  // Add distances travelled.	
	z += deltaZ; 
	


	//*********************************************************************//
	//Now handle collision with the floor.   Collisions with any walls can //
	//be handled in exactly the same way.								   //
	//*********************************************************************//
      	if (y < (groundY + r))
	{
		ySpeed = -ySpeed*bouncyness;	//Bounce back.
		y = groundY+r;					//Ensure above test false next 
										//frame.  A cheat?  Maths os discrete
										//and doesn't give the whole story.


		//******************************************************************//
		// That was the maths.   Now make a sound.  This is how to play a	//
		// simple wav file.													//
		//******************************************************************//
		PlaySound(L"Media\\boing.wav", NULL, SND_ASYNC | SND_NOSTOP);
	}



	//*******************************************************************//
	//This is art.   If the ball is within 2* radius of the floor, squash//
	//it.																 //
	//*******************************************************************//
	if (y < (groundY+r*2))
	{
		float scaleY = (y - groundY);
		SetScale(1, scaleY, 1);
	}
	else SetScale(1, 1, 1);   //Not very good (fast) to do this every frame.

	ry = spin * xSpeed * xSpeed;

	RotateBy(0, ry, 0);	//spin a bit iffy
}
