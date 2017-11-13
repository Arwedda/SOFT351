//**************************************************************************//
// Class to implement a bouncing ball.     It is not yet finished.  None of //
// Nigel's demos ever are :-(												//
//                                                                          //
// All a but messier than Java - you declare the members and prototypes for //
// the methods in the header file, but the implementation of the methods is //
// given in the "cpp" file.                                                 //
//**************************************************************************//

//**************************************************************************//
// This code is copyright of Dr Nigel Barlow, lecturer in computing,	   //
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


#include "thing3D.h"

#ifndef ballH  //The usual "have we seen this before?" guard
#define ballH

class Ball : public Thing3D
{
	public:

		float r;						//Radius..
		float xSpeed, ySpeed, zSpeed;   //Separate speed into these 3 components.
		ULONG createTime;				//Time created.

		//******************************************************************//
		// Newton physics variables, just gravity as it happens.  Surely if	//
		// anything shoud be global, not a property of the ball, g should	//
		// be global; let's keep it simple.									//
		//******************************************************************//
		float g;		 
		float groundY;		//Position of the ground.  Should be global?
		float bouncyness;
		float mass;
		   
		float revPerMin;		// revolution per minute, to calculate spin
		float airDensity;		// air at 1 atm pressure, at around 20 degrees Celcius
		float dragCoefficient;	// drag coefficient of a spherical body





		//******************************************************************//
		// Constructor.  Force the parent constructor to execute, then		//
		// create the default shaders.										//
		//******************************************************************//
		Ball(ID3D11Device           *pRenderingDevice,
				ID3D11DeviceContext *pImmediateContext);

		virtual void setXYZspeed(float newXSpeed, float newYSpeed,  
			                     float newZSpeed);
		virtual void Bounce();
};


//---------------------------------------------------------------------------
#endif
