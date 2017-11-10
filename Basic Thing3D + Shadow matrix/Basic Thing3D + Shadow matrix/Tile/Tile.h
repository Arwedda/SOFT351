//**************************************************************************//
// Implements a flat tile in 3d space.  The doBehaviour() method is all		//
// that changes between this and its parent class.							//
//																			//
//	It is a child of the  class												//
// FlatThing3D.																//
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

#include "FlatThing3D.h"
#include "DXUT.h"



#ifndef TileH     //These are termed "guards", can you see what they do?
#define TileH	 






class Tile : public FlatThing3D
{


	//**********************************************************************//
	// Public instance variables.											//
	//**********************************************************************//
	public:
		XMFLOAT4		materialOriginalDiffuseColour;		//w value unusedd.	  
		bool		    doEffect;
	

	//**********************************************************************//
	// Constructors and destructors.										//
	//**********************************************************************//

	public:
	  Tile(ID3D11Device        *pRenderingDevice,
		   ID3D11DeviceContext *pImmediateContext);
      ~Tile();									//Destructor.


	virtual void DoBehaviour();


};    //Must be a ";" here in C++ - weird, eh?   Nigel


#endif	//End of guard.

