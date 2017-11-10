//**************************************************************************//
// Class to implement a Thing3d with a ppotlight.  It is a child of a		//
// Thing3D.																	//
//**************************************************************************//

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
//																			//
//	3:	You should correct at least 10% of the typig abd spekking errirs.   //
//**************************************************************************//


#include "Thing3DAbstract.h"
#include "Thing3D.h"



#ifndef Thing3D_SHADOW_H     //These are termed "guards", can you see what they do?
#define Thing3D_SHADOW_H	 






//**************************************************************************//
// Many of these member variables are public and have to be set by writing  //
// to the variable.   OO programmers would have us write many more          //
// setThisandThat(...) methods.                                             //
//**************************************************************************//
class Thing3DWithShadow : public Thing3D
{
	//**********************************************************************//
	// Public stuff.														//
	//**********************************************************************//
	public:




		//******************************************************************//
		// Constructor.  Force the parent constructor to execute, then		//
		// create the default shaders.										//
		//******************************************************************//
		Thing3DWithShadow(ID3D11Device        *pRenderingDevice,
				             ID3D11DeviceContext *pImmediateContext);
		~Thing3DWithShadow();									//Destructor.


		virtual void RenderForMyImplementation(ID3D11Device        *pRenderingDevice,
					                           ID3D11DeviceContext *pImmediateContext);	//Draw yourself 
																						//for whatever you are



};	// End of classy class definition. 
	// Must be a ";" here in C++ - weird, eh?   Nigel


#endif	//End of guard.

