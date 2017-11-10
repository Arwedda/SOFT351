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



#include "Thing3DWithShadow.h"

//**************************************************************************//
// Constructor.  Force the parent constructor to execute, then create the   //
// default shaders.															//
//**************************************************************************//
Thing3DWithShadow::Thing3DWithShadow(ID3D11Device        *pRenderingDevice,
			                               ID3D11DeviceContext *pImmediateContext) 
			               : Thing3D(pRenderingDevice, pImmediateContext)
{


}



//**************************************************************************//
// Implementation of an abstract method in the parent class.  Render ourself//
// for whatever we are in this particular implementation, which is a mesh.	//
//**************************************************************************//
void Thing3DWithShadow::RenderForMyImplementation(ID3D11Device        *pRenderingDevice,
					                                 ID3D11DeviceContext *pImmediateContext) 											
{


	//****************************************************************************//
	// Render myself first.														  //
	//****************************************************************************//
	Thing3D::RenderForMyImplementation(pRenderingDevice, pImmediateContext);



	//****************************************************************************//
	// Use D3DXMatrixShadow to create a FAKE shadow.   This matrix flattens a	  //
	// geometry onto a plane, an effective fake if your ground is flat.   If you  //
	// have undulating terrain, you won't be able to use this method :-(		  //
	//																			  //
	// Furthermore, the plane is supposed to correspond to the floor, so the	  //
	// shadow should NOT fall correctly on the back wall.   Do you think it does? //
	//																			  //
	// We use some alpha belnding to make the shadow look more realistic.		  //
	//																		 	  //
	// There are some odd effects here.   If you initially place the tiger at	  //
	// (0, 0, 0) (done in this pplication's constructor), all is fine.   Try	  //
	// putting the tiger at (0, 0, 5)......										  //
	//																			  //
	// This demo uses a separate translation matrix to move the shadow.   I am	  //
	// unclear of why i have to do this to get the shadow in the right place.  I  //
	// suspect my equations of my plane are up the spout!  - Nigel.				  //
	//																			  //
	// It would be considerably better to put much of this code inside the	      //
	// object Thing3D.	Then the Thing3D would be able to cast its own shadows	  //
	// -Nigel																	  //
	//****************************************************************************//
	XMMATRIX     matShadow, matShadowTranslate, matShadowTransformed;
	XMVECTOR     vecLight  = XMVectorSet(1, 1, 0, 0);	//Points at light. Point light.  
	XMVECTOR	 floorPlane= XMVectorSet(0, 1, 0, 0);	//These are the coefficients for the
														//plane equation ax + by + cz + d = 0;
														//The plane is not currently in the 
														//correct place!

	vecLight           = XMVector3Normalize(vecLight);  //Belt and braces.
	matShadow          = XMMatrixShadow(floorPlane, vecLight);		
	matShadowTranslate = XMMatrixTranslation(x, y-0.5f, z);
		

	//**********************************************************************//
	// There is some issue with the overloaded multiply of an XMMatrix that //
	// the good ond D3DXMATRIX never used to have.   If you use the			//
	// overloaded multiply on an object's instance variables it goes bang!	//
	// Answers to Nigel please!  There may be a prize...					//
	//**********************************************************************//
	XMMATRIX matWorldlocal              = matWorld;
	XMMATRIX matViewLocal               = matView;
	XMMATRIX matProjectionLocal         = matProjection;
	XMMATRIX matWorldViewProjectionLocal;
				
	matShadowTransformed        = matWorldlocal * matShadow * matShadowTranslate;
	matWorldViewProjectionLocal = matShadowTransformed * matViewLocal * matProjectionLocal;


	//******************************************************************//    
	// Update shader variables.  We must update these for every mesh	//
	// that we draw (well, actually we need only update the position	//
	// for each mesh, think hard about this - Nigel						//
	//																	//
    // We pass the parameters to it in a constant buffer.  The buffer	//
	// we define in this module MUST match the constant buffer in the	//
	// shader.															//
	//																	//
	// It would seem that the constant buffer we pass to the shader must//
	// be global, well defined on the heap anyway.  Not a local variable//
	// it would seem.													//
	//******************************************************************// 
	Thing3DVS_PS_Pair::CBMatrices      cbMatrices;
	cbMatrices.matWorld               = XMMatrixTranspose(matWorldlocal);
	cbMatrices.matWorldViewProjection = XMMatrixTranspose(matWorldViewProjectionLocal);
	pThing3DVS_PS_Pair->UpdateCBMatrices(pImmediateContext, &cbMatrices);								

	pImmediateContext->PSSetSamplers( 0, 1, &pThing3DVS_PS_Pair->pSamLinear );

	
	//**************************************************************************//
	// Render the mesh.															//
	//**************************************************************************//
	//Get the mesh
    //IA setup

	pImmediateContext->IASetInputLayout( pThing3DVS_PS_Pair->pVertexLayout );
    UINT Strides[1];
    UINT Offsets[1];
    ID3D11Buffer* pVB[1];
    pVB[0] = mesh.GetVB11( 0, 0 );
    Strides[0] = ( UINT )mesh.GetVertexStride( 0, 0 );
    Offsets[0] = 0;
    pImmediateContext->IASetVertexBuffers( 0, 1, pVB, Strides, Offsets );
	pImmediateContext->IASetIndexBuffer( mesh.GetIB11( 0 ), mesh.GetIBFormat11( 0 ), 0 );
	
    
	UINT                      numSubsets                 =  mesh.GetNumSubsets( 0 );
	boolean                   mustRestoreOriginalTexture = false;
    ID3D11ShaderResourceView  *pDiffuseRV				 = NULL;
	SDKMESH_SUBSET			  *pSubset                   = NULL;
	D3D11_PRIMITIVE_TOPOLOGY  PrimType;
	


	//**************************************************************************//
	// Render each subset.														//
	//**************************************************************************//
	for( UINT subset = 0; subset < numSubsets; ++subset )
    {
		pSubset = NULL;
        
		// Get the subset
        pSubset = mesh.GetSubset( 0, subset );

        PrimType = CDXUTSDKMesh::GetPrimitiveType11( ( SDKMESH_PRIMITIVE_TYPE )pSubset->PrimitiveType );
		pImmediateContext->IASetPrimitiveTopology( PrimType );

		//**************************************************************************//
		// Should there be more than one texture, we put it into each one into the  //
		// same slot in shader memory, which is going to be slow.					//
		//**************************************************************************//
		//if (subset > 0)	// not yet working
		{
			pImmediateContext->PSSetShaderResources( textureNumber, 1, &pDiffuseRV );
			mustRestoreOriginalTexture = true;
		}

		pImmediateContext->DrawIndexed( ( UINT )pSubset->IndexCount, 0, ( UINT )pSubset->VertexStart );
    }
}





//**************************************************************************//
// Destructor.   We should release some Dx stuff here.   It is NOT complete!//
//**************************************************************************//
Thing3DWithShadow::~Thing3DWithShadow()
{
}