//**************************************************************************//
// Class to implement a Thing3d.     It is a child of the abstract class	//
// "Thing3DAbstract.h.														//
//																			//
// This one uses the Microsoft sample code to load itself from a Wavefront  //
// ".obj" file.	 Thanks Microsoft!											//
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
#include "meshloaderobj10.h"
#include "SDKmesh.h"



#ifndef Thing3DObjH     //These are termed "guards", can you see what they do?
#define Thing3DObjH	 


//**************************************************************************//
// Many of these member variables are public and have to be set by writing  //
// to the variable.   OO programmers would have us write many more          //
// setThisandThat(...) methods.                                             //
//**************************************************************************//

class Thing3DObj : public Thing3DAbstract
{

	//**********************************************************************//
	// Public instance variables.											//
	//**********************************************************************//

	public:
		CMeshLoader10  objMesh;

	//**********************************************************************//
	// Public methods.														//
	//**********************************************************************//

	public:
      Thing3DObj();									//Constructors.
	  Thing3DObj(ID3D10Device *renderingDevice);
      ~Thing3DObj();									//Destructor.



	//**********************************************************************//
	// Implementation of abstract methods in parent class.					//
    //**********************************************************************//

	public:
      virtual void create();
	  virtual void renderForMyImplementation();		   //Draw yourself 
													   //for whatever you are
	  virtual void doBehaviour();



      //**********************************************************************//
	  // Load the mesh from an sdkmesh file.  This must be called AFTER	      //
	  // createEffect().											 		  //
	  //**********************************************************************//
	 
	  virtual void createMesh(LPCTSTR OBJMeshFileName);//Load graphic from file.

	  private:
		  void renderSubset( UINT iSubset );


};    //Must be a ";" here in C++ - weird, eh?   Nigel


#endif	//End of guard.

