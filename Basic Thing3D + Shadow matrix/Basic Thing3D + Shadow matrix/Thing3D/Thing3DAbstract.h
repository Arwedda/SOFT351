//**************************************************************************//
// Class to implement a Thing3d.     It is not yet finished.                //
//                                                                          //
// All a but messier than Java - you declare the members and prototypes for //
// the methods in the header file, but the implementation of the methods is //
// given in the "cpp" file.                                                 //
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

#include "DXUT.h"
#include "DXUTmisc.h"
#include "SDKmisc.h"
#include "SDKmisc.h"
#include "SDKMesh.h"
#include <xnamath.h>
#include "Thing3DVS_PS_Pair.h"
#include "..\Nig.h"

#ifndef Thing3DHAbstract_H   //Guards, as usual.
#define Thing3DHAbstract_H








//**************************************************************************//
// Many of these member variables are public and have to be set by writing  //
// to the variable.   OO programmers would have us write many more          //
// setThisandThat(...) methods.                                             //
//**************************************************************************//

class Thing3DAbstract
{

	//**********************************************************************//
	// Public instance variables.  OO purists will want to make these		//
	// private and write the setThis() and getThat() methods.				//
	//**********************************************************************//  
   public:					//Not quite like Java, but the same meaning.
      float x, y, z;        //Position.
      float speed;          //Yes - it goes!
      float rx, ry, rz;     //Rotation about x, y, z axes.
      float sx, sy, sz;     //Scales in x, y, z directions.
      
      float frameTime;						//You must specify this, I use it
											//to calculate the elapsed time 
											//between frames and such

      bool  buffersAndtexturesSet;			// Performance optimisation, don't
											// set up vertex and index buffers, 
											// textures if this has already 
											// been done.
 
	  //**********************************************************************//
	  // Now we hit DirectX components.  First general maths stuff.		   //
	  //**********************************************************************//
   public:
	   XMVECTOR initVecDir;		 //Set once only.   Initial direction.
	   XMVECTOR currentVecDir;	 //Used for moveForward and culling in
								 //lated demos.
	  
	  
	  
	  
	  
	  //***************************************************************//	  
	  //Rotation about x, y and z matrices, and a translation matrix.  //
      //I have made these members of this class so that we need only   //
      //compute them once a frame, but if you look at the "moveForward"//
      //method, you will see that the matrices are re-calculated.   I  //
      //have no idea why...      Nigel.                                //
	  //***************************************************************//
	  XMMATRIX  matAllRotations;   //Used for moveForward.
      XMMATRIX  matRotateX;
      XMMATRIX  matRotateY;
      XMMATRIX  matRotateZ;
	  XMMATRIX	matScale;
      XMMATRIX  matTranslate;
      
	  XMMATRIX  matWorld;			//We need to pass the world view and
	  XMMATRIX  matView;			//projection matrices to the snader.
      XMMATRIX  matProjection;
	  XMMATRIX	matWorldViewProjection;

	  UINT      textureNumber;



	


	  //**********************************************************************//
	  // It it likely that there is going to be one shader that everything    //
	  // shares.															  //
	  //**********************************************************************//
	  Thing3DVS_PS_Pair *pThing3DVS_PS_Pair;



	  //**********************************************************************//
	  // Material properties; diffuse reflection only here.					  //
  	  //**********************************************************************//
	  XMFLOAT4		materialDiffuseColour;		//w value unused.




		//**********************************************************************//
		// This is where encapsulation falls down some.  The shader file has its//
		// lighting attributes.  This doesn't really make sense surely light is //
		// a globl thing, not an attribute of an object?  It makes partial sense//
		// here, as I am assuming that in a simple world. all objects will share//
		// the same shader.														//
		//**********************************************************************//
		XMVECTOR		vecLightDirection;				//w value unused.
		XMFLOAT4		lightDiffuseColour;				//w value unused.
		XMFLOAT4		LightAmbientColour;				//w value unused.







	//**********************************************************************//
	// Public methods.														//
	//**********************************************************************//

   public:
	  Thing3DAbstract(ID3D11Device        *pRenderingDevice,
					  ID3D11DeviceContext *pImmediateContext);

      ~Thing3DAbstract();    //Destructor.

      //*********************************************************************//
      //This lot all return void, but it would be better to have them return //
      //an HRESULT to indicate success/failure.   Or even throw an exception.//
	  //                                                                     //
      // There should be more of these to set the speed, material etc, but   //
      // as yet, this is all there is.                                       //
      //*********************************************************************//


      //*********************************************************************//
	  // ABSTRACT METHODS - you must implelemt them in a subclass.			 //
	  //																	 //
	  //*********************************************************************//
      // Various versions of "initGeometry".   The one that takes the name of//
	  // a mesh is the most useful.											 //
      // here I handle both :-)                Nigel.                        //
      //*********************************************************************//
      virtual void Create(ID3D11Device        *pRenderingDevice,
					      ID3D11DeviceContext *pImmediateContext)				     = 0;
	  virtual void RenderForMyImplementation(ID3D11Device       *pRenderingDevice,
									        ID3D11DeviceContext *pImmediateContext)  = 0;  //Draw yourself 
														                                  //for whatever you are


      //*********************************************************************//
      // This one is for you to fill in the bevaviour of the object.         //
      //*********************************************************************//
      virtual void DoBehaviour() = 0;

     


	  //********************************************************************//
	  // Set the VS / PS shader pair.  It is assumed that many instances of //
	  // this object will share the same shader.							//
	  //********************************************************************//
	  virtual void SetVS_PS_ShaderPair(Thing3DVS_PS_Pair *pShader);




      //********************************************************************//
      // Set the object's initial direction - used by the moveForward       //
      // method.                                                            //
      //********************************************************************//
      virtual void SetInitialDirection(float initX, float initY, float initZ);



      //********************************************************************//
      // Move to an absolute position.                                      //
      //********************************************************************//
   	  virtual void MoveTo(float newX, float newY, float newZ);


      //********************************************************************//
      // Move relative to the current position.                             //
      //********************************************************************//
   	  virtual void MoveBy(float byX,  float byY,  float byZ);

      //*********************************************************************//
      // Move the object forward in the direction it is pointing, at the     //
      // speed it is going.  That is the difficult one!                      //
      //*********************************************************************//
   	  virtual void MoveForward();


      //********************************************************************//
      // Rotate to an absolute angle.                                       //
      //********************************************************************//
   	  virtual void RotateTo(float newRX,  float newRY,  float newRZ);



      //********************************************************************//
      // Rotate relative to current orientation.                            //
      //********************************************************************//
   	  virtual void RotateBy(float byRX,   float byRY,   float byRZ);


      //********************************************************************//
      // Set the scale of the object.                                       //
      //********************************************************************//
   	  virtual void SetScale(float newsx,   float newsy,   float newsz);


      //********************************************************************//
      // Draw yourself - and do the matrix stuff as well...                 //
      //********************************************************************//
      virtual void Render(ID3D11Device        *pRenderingDevice,
						  ID3D11DeviceContext *pImmediateContext);  
	  //The virtual methods are replaced if a child //
                              //object has a method of the same name.       //
                              //Ask Nigel.                                  //


     
      //********************************************************************//
      // Set up the world matrix.											//
      //********************************************************************//   
	  virtual void SetUpWorldMatrix();


	  //*********************************************************************//
	  // This is an extremely simple method to test whether two things		 //
	  // intersect.  And it only works with objects that are of its own kind //
	  // (or children of its own kind).										 //
	  //																	 //
	  // So go write something better.										 //
	  //*********************************************************************//
	  virtual bool IntersectsWith(Thing3DAbstract *thing);



    //**********************************************************************//
	// Private and protected methods.										//
	//**********************************************************************//

	protected:
		void Init(ID3D11Device        *pRenderingDevice,
				  ID3D11DeviceContext *pImmediateContext); // Used by constructors.	


		

};    //Must be a ";" here in C++ - weird, eh?   Nigel


   #endif

