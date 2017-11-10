//**************************************************************************//
// Class to implement a Thing3DAbstract.     It is not yet finished.        //
//																			//
// This class is an ABSTRACT class; you cannot make it with a new().  You	//
// must sub-class it and omplement the abatract methods in the sub class.	//
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
#include "DXUT.h"
#include "DXUTgui.h"
#include "DXUTmisc.h"
#include "DXUTSettingsDlg.h"
#include "SDKmisc.h"
#include "SDKmesh.h"
#include <string.h>
#include <wchar.h>
#include <exception>	//Std stuff; no ".h"






//**************************************************************************//
// Constructors.															//
//**************************************************************************//
Thing3DAbstract::Thing3DAbstract(ID3D11Device         *pRenderingDevice,
								 ID3D11DeviceContext  *pImmediateContext)
{
	Init(pRenderingDevice, pImmediateContext);
}





//**************************************************************************//
// Used by constructors.													//
//**************************************************************************//
void Thing3DAbstract::Init(ID3D11Device         *pRenderingDevice,
						   ID3D11DeviceContext  *pImmediateContext)
{
   HRESULT hr           = 0;	// needed for V macro.
   x  = y  = z  = speed = 0.0f;
   rx = ry = rz         = 0.0f;
   sx = sy = sz         = 1.0f;
   initVecDir           = XMVectorSet(1, 0, 0, 1);	//W unused
   frameTime	        = 0.001;

   materialDiffuseColour = XMFLOAT4(1, 1, 1, 1);
   lightDiffuseColour    = XMFLOAT4(0.1, 0.1, 0.1, 0.1);
   vecLightDirection     = XMVectorSet(1, 1, -2, 1);	//W unused.

   matRotateX = matRotateY = matRotateZ = matTranslate 
	   = matProjection = matView = matAllRotations 
	   = matWorld = matWorldViewProjection = XMMatrixIdentity();

   textureNumber = 0;
   SetUpWorldMatrix();
}



//**************************************************************************//
// Destructor.   We should release some Dx stuff here.						//
//**************************************************************************//
Thing3DAbstract::~Thing3DAbstract()
{
		//For you to think about.  Release all Dx stuff.
}


//**************************************************************************//
// Set the VS / PS shader pair.  It is assumed that many instances of this	//
// object will share the same shader.										//
//**************************************************************************//
void Thing3DAbstract::SetVS_PS_ShaderPair(Thing3DVS_PS_Pair *pShader)
{
	pThing3DVS_PS_Pair = pShader;
}




//**************************************************************************//
// Move to an absolute position.                                            //
//**************************************************************************//

void Thing3DAbstract::MoveTo(float newX, float newY, float newZ)
   {
   x = newX; y = newY; z = newZ;
   }


//**************************************************************************//
// Move relative to the current position.                                   //
//**************************************************************************//

void Thing3DAbstract::MoveBy(float byX,  float byY,  float byZ)
   {
   x += byX; y += byY; z += byZ;
   }


//**************************************************************************//
// Rotate to an absolute angle.                                             //
//**************************************************************************//

void Thing3DAbstract::RotateTo(float newRX,  float newRY,  float newRZ)
   {
   rx = newRX; ry = newRY; rz = newRZ;
   }


//**************************************************************************//
// Rotate relative to current orientation.                                  //
//**************************************************************************//

void Thing3DAbstract::RotateBy(float byRX,  float byRY,  float byRZ)
   {
   rx += byRX; ry += byRY; rz += byRZ;

   float twoPi = XM_PI*2;

   if (rx > twoPi) rx -= twoPi;         //Just to stop the rotation angle
   if (ry > twoPi) ry -= twoPi;         //going above/below one circle.
   if (rz > twoPi) rz -= twoPi;

   if (rx < 0) rx += twoPi;
   if (ry < 0) ry += twoPi;
   if (rz < 0) rz += twoPi;
   }



//*************************************************************************//
// Set the scale of the object.                                            //
//*************************************************************************//
void Thing3DAbstract::SetScale(float newsx,   float newsy,   float newsz)
   {
   sx = newsx;   sy = newsy;   sz = newsz;   
   }



//**************************************************************************//
// Set the object's initial direction - used by the moveForward method.     //
//**************************************************************************//
void Thing3DAbstract::SetInitialDirection(float initX, float initY, float initZ)
   {
   initVecDir = XMVectorSet(initX, initY, initZ, 0);
   }




//**************************************************************************//
// Move the object forward in the direction it is pointing, at the speed it //
// is going.                                                                //
//                                                                          //
// If anybody can come up with a better solution, see Nigel!                //
//                                                                          //
// We start by creating a vector pointing down the x axis (default is       //
// 1, 0, 0), then multiply this vector by the combined rotation matrices    //
// (x, y, z) rotations.                                                     //
//                                                                          //
// We normalise the resultane vector (i.e. give it a magnitude of 1),       //
// extract the x, y, z components, and multiply them by the object's speed. //
//                                                                          //
// I haven't attampted to make this code independent of the frame rate yet! //
//                                                                          //
// Nigel.                                                                   //
//**************************************************************************//

void Thing3DAbstract::MoveForward() 
{
    

	//*******************************************************************//
	// Help!   I need to re-create the rotation matrices here; I have no //
    // idea why.   The whole point of making the matrices members of this//
    // class was so that I need calculate them only once.   Remove the   //
    // lines to the ------------- and this should still work, but doesn't//
    // - Nigel.                                                          //
	//*******************************************************************//
	SetUpWorldMatrix();

	//**********************************************************************//
	// There is some issue with the overloaded multiply of an XMMatrix that //
	// the good ond D3DXMATRIX never used to have.   If you use the			//
	// overloaded multiply on an object's instance variables it goes bang!	//
	// Answers to Nigel please!  There may be a prize...					//
	//**********************************************************************//
	XMMATRIX matAllRotationsLocal = matAllRotations;
	XMVECTOR initVecDirLocal      = initVecDir;
	XMVECTOR currentVecDirLocal   = currentVecDir;

	currentVecDirLocal = XMVector3TransformCoord(initVecDirLocal, 
												 matAllRotationsLocal);
	currentVecDirLocal = XMVector3Normalize(currentVecDirLocal);


    float adjustedSpeed = speed*frameTime;
    
    x +=  XMVectorGetX(currentVecDirLocal) * adjustedSpeed;  
	y +=  XMVectorGetY(currentVecDirLocal) * adjustedSpeed;  
    z +=  XMVectorGetZ(currentVecDirLocal) * adjustedSpeed;

	currentVecDir = currentVecDirLocal;
}




//**************************************************************************//
// Draw yourself - and do the matrix stuff as well...                       //
//**************************************************************************//
void Thing3DAbstract::Render(ID3D11Device        *pRenderingDevice,
					         ID3D11DeviceContext *pImmediateContext)
{

	try
	{
		SetUpWorldMatrix();              //Do the movement stuff.
		RenderForMyImplementation(pRenderingDevice, pImmediateContext);
   }
   catch (std::exception *ex) { }
}








//**************************************************************************//
// Set up the world matrix.													//
//**************************************************************************//
void Thing3DAbstract::SetUpWorldMatrix()
{
	try
	{
		//Rotation about x, y and z matrices, and a translation matrix.
		matRotateX = XMMatrixRotationX(rx);
		matRotateY = XMMatrixRotationY(ry);
		matRotateZ = XMMatrixRotationZ(rz);
   

		//**********************************************************************//
		// There is some issue with the overloaded multiply of an XMMatrix that //
		// the good ond D3DXMATRIX never used to have.   If you use the			//
		// overloaded multiply on an object's instance variables it goes bang!	//
		// Answers to Nigel please!  There may be a prize...					//
		//**********************************************************************//
		XMMATRIX matAllRotationsLocal = XMMatrixRotationRollPitchYaw(rx, ry, rz);

		//Now scale it before we translate it.
		XMMATRIX matScaleLocal     = XMMatrixScaling(sx, sy, sz);
		XMMATRIX matTranslateLocal = XMMatrixTranslation( x, y, z);

		//**************************************************************************//
		// If we want to both rotate and translate, we must multiply both matrices	//
		// together.   This multiplies matRotateY and matTranslate to produce		//
		// matTransformed.   The order of multiplication is important.   Try		//
		// reversing it and see if you can understand what you get.					//
		//**************************************************************************//
		
		
		XMMATRIX matWorldLocal = matAllRotationsLocal * matScaleLocal * matTranslateLocal; 

		matAllRotations = matAllRotationsLocal;
		matTranslate    = matTranslateLocal;
		matScale        = matScaleLocal;
		matWorld        = matWorldLocal;
	}
	catch (std::exception) { }
}









//**************************************************************************//
// This is an extremely simple method to test whether two things intersect. //
// And it only works with objects that are of its own kind (or children of  //
// its own kind).															//
//																			//
// So go write something better.											//
//**************************************************************************//
bool Thing3DAbstract::IntersectsWith(Thing3DAbstract *thing)
{
	float dx = x - thing->x;
	float dy = y - thing->y;
	float dz = z - thing->z;

	dy = 0;
	float dist = (float) sqrt(dx*dx + dy*dy + dz*dz);

	float averageScale = (sx + sy + sz) / 3.0f;

	if (averageScale > dist) return true;


	return false;
}







