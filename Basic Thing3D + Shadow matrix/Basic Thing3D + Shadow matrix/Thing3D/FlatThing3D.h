//**************************************************************************//
// Implements a flat thing in 3d space, which can be used as a background	//
// - or anything flat :-)													//
//																			//
//	It is a child of the abstract class										//
// "Thing3DAbstract.h.														//
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



#ifndef FlatThing3D_H     //These are termed "guards", can you see what they do?
#define FlatThing3D_H	 


class FlatThing3D : public Thing3DAbstract
{

	//**********************************************************************//
	// Public instance variables.											//
	//**********************************************************************//
	public:
		ID3D11Buffer			  *pVertexBuffer;
		ID3D11Buffer			  *pIndexBuffer;
		ID3D11ShaderResourceView  *ptxtResourceViewInShader;
		ID3D11InputLayout		  *pVertexLayout;

		//**************************************************************************//
	    // Structure to define a Vertex.  Thid is linked in with the shader we use, //
		// and this won't work unless you have a shader with a posotion, normal		//
		// vector and textureUV coordinates.										//
		//**************************************************************************//
		struct SimpleFlatThingVertex
		{
			XMFLOAT3 pos;
			XMFLOAT3 normal;
			XMFLOAT2 texUV;
		};


	//**********************************************************************//
	// Implementation of abstract methods in parent class.					//
    //**********************************************************************//
	public:
      virtual void Create(ID3D11Device        *pRenderingDevice,
					      ID3D11DeviceContext *pImmediateContext);
	  virtual void RenderForMyImplementation(ID3D11Device        *pRenderingDevice,
					                         ID3D11DeviceContext *pImmediateContext);	//Draw yourself 
																						//for whatever you are
	  virtual void DoBehaviour();


	//**********************************************************************//
	// Constructors and destructors.										//
	//**********************************************************************//
	public:
	  FlatThing3D(ID3D11Device        *pRenderingDevice,
			      ID3D11DeviceContext *pImmediateContext);
      ~FlatThing3D();									//Destructor.


	//**********************************************************************//
	// Public methods.														//
	//**********************************************************************//
	public:



    //**********************************************************************//
	// Protected and private methods.										//
	//**********************************************************************//
	protected:
	  void Init(); // Used by constructors.	



};    //Must be a ";" here in C++ - weird, eh?   Nigel


#endif	//End of guard.

