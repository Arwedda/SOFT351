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


#include "Tile.h"



//**************************************************************************//
// Constructors.															//
//**************************************************************************//
Tile::Tile(ID3D11Device        *pRenderingDevice,
		   ID3D11DeviceContext *pImmediateContext)
		   : FlatThing3D(pRenderingDevice, pImmediateContext)

{
	doEffect					  = false;
	materialOriginalDiffuseColour = XMFLOAT4(1, 1, 1, 1);
}


//**************************************************************************//
// Destructor.   We should release some Dx stuff here.						//
//**************************************************************************//

Tile::~Tile()
{
	//For you to think about...	
}



//**************************************************************************//
// Do something interesting.  Well, something anyway.  Here we reduce the   //
// difuse reflection, just to show that something has happened.				//
//**************************************************************************//

void Tile::DoBehaviour()
{
	if (doEffect) 
	{
		materialDiffuseColour.x += 0.01;
		if (materialDiffuseColour.x > materialOriginalDiffuseColour.x)
		{
			materialDiffuseColour.x = materialOriginalDiffuseColour.x;
			doEffect = false;
		}
	}
}