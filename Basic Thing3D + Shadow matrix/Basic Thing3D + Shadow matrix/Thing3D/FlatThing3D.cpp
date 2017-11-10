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

#include "FlatThing3D.h"
#include "..\Nig.h"







//**************************************************************************//
// Constructors.															//
//**************************************************************************//

FlatThing3D::FlatThing3D(ID3D11Device        *pRenderingDevice,
					     ID3D11DeviceContext *pImmediateContext) 
					   : Thing3DAbstract(pRenderingDevice, pImmediateContext)
{
	Init();
}



//**************************************************************************//
// Used by constructors.													//
//**************************************************************************//
void FlatThing3D::Init()
{
   x  = y  = z  = speed = 0.0f;
   ry = rz              = 0.0f;
   rx					= XMConvertToRadians(270.0f);		//Orientation for background.
   sx = sy = sz         = 1.0f;
   initVecDir			= XMVectorSet(1, 0, 0, 0);
   frameTime			= 0.001;


   materialDiffuseColour = XMFLOAT4(1, 1, 1, 1);
}




//**************************************************************************//
// Destructor.   We should release some Dx stuff here.   It is NOT complete!
//**************************************************************************//

FlatThing3D::~FlatThing3D()
{

}						









//**************************************************************************//
// Create vertex and index buffers with the geometry of two back to back	//
// triangles.																//					
//**************************************************************************//
void FlatThing3D::Create(ID3D11Device        *pRenderingDevice,
					     ID3D11DeviceContext *pImmediateContext)
{
	HRESULT hr;		//Used by the V() macro.  


	UINT numElements = ARRAYSIZE( thing3DAvstractVertexLayoutDescription );
	
	hr = pRenderingDevice->CreateInputLayout( thing3DAvstractVertexLayoutDescription, 
		                                      numElements, 
											  pThing3DVS_PS_Pair->pVertexShaderBuffer->GetBufferPointer(),
                                              pThing3DVS_PS_Pair->pVertexShaderBuffer->GetBufferSize(), 
											  &pVertexLayout );

 	V(hr);

    // Set the input layout
    pImmediateContext->IASetInputLayout( pVertexLayout );

    // Create VERTEX buffer
    SimpleFlatThingVertex vertices[] =
    {
		{ XMFLOAT3( -1.0f, 0.0f, -1.0f ), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2( 0.0f, 0.0f ) },
        { XMFLOAT3( 1.0f, 0.0f, -1.0f ),  XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2( 1.0f, 0.0f ) },
        { XMFLOAT3( 1.0f, 0.0f, 1.0f ),   XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2( 1.0f, 1.0f ) },
        { XMFLOAT3( -1.0f, 0.0f, 1.0f ),  XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2( 0.0f, 1.0f ) },
    };

    D3D11_BUFFER_DESC bd;
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof( SimpleFlatThingVertex ) * 4;
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bd.CPUAccessFlags = 0;
    bd.MiscFlags = 0;
    D3D11_SUBRESOURCE_DATA InitData;
    InitData.pSysMem = vertices;
	hr = pRenderingDevice->CreateBuffer( &bd, &InitData, &pVertexBuffer );
    V( hr ) 
     
    
    //D3D11_BUFFER_DESC bd;
    bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(UINT)* 6;
    bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    bd.CPUAccessFlags = 0;
    bd.MiscFlags = 0;
    //D3D11_SUBRESOURCE_DATA InitData;
    InitData.pSysMem = vertices;
	
    // Create index buffer
    UINT indices[] =
    {
        3,1,0,
        2,1,3    
	};

	
	InitData.pSysMem = indices;
	hr = pRenderingDevice->CreateBuffer( &bd, &InitData, &pIndexBuffer );
 	
}




//**************************************************************************//
// Implementation of an abstract method in the parent class.  Render ourself//
// for whatever we are in this particular implementation, which is a mesh.	//
//**************************************************************************//
void FlatThing3D::RenderForMyImplementation(ID3D11Device        *pRenderingDevice,
					                        ID3D11DeviceContext *pImmediateContext) 											
{

	//**********************************************************************//
	// There is some issue with the overloaded multiply of an XMMatrix that //
	// the good ond D3DXMATRIX never used to have.   If you use the			//
	// overloaded multiply on an object's instance variables it goes bang!	//
	// Answers to Nigel please!  There may be a prize...					//
	//**********************************************************************//
	XMMATRIX matWorldlocal       = matWorld;
	XMMATRIX matViewLocal        = matView;
	XMMATRIX matProjectionLocal  = matProjection;

	XMMATRIX matWorldViewProjectionLocal = matWorldlocal * matViewLocal * matProjectionLocal;

	matWorld = matWorldlocal;




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

	Thing3DVS_PS_Pair::CBLighting cbLighting;
	cbLighting.lightDiffuseColour = lightDiffuseColour;
	cbLighting.lightDiffuseColour = lightDiffuseColour;
	cbLighting.materialDiffuseColor = materialDiffuseColour;
	cbLighting.vecLightDir          = XMVector3Normalize(vecLightDirection);
	pThing3DVS_PS_Pair->UpdateCBLighting(pImmediateContext, &cbLighting);

	
	//**********************************************************************//
	// I suspect you will think this is pretty horrible too.   Each			//
	// instance of this obkect has its own vertex, index buffers etc.		//
	// however if they only need one and it has already been set, skip this	//
	// piece of code.  Switching all this stuff happers perfoemance		    //
	//**********************************************************************//
	if (!buffersAndtexturesSet)
	{
		pImmediateContext->PSSetSamplers( 0, 1, &pThing3DVS_PS_Pair->pSamLinear );
		pThing3DVS_PS_Pair->SetupShadersAndConstantbuffers(pImmediateContext);



		//*******************************************************************//
		// Set Vertex buffer and index buffer.								 //
		//*******************************************************************//
		pImmediateContext->IASetInputLayout( pThing3DVS_PS_Pair->pVertexLayout );

		UINT stride = sizeof( SimpleFlatThingVertex );
		UINT offset = 0;
		pImmediateContext->IASetVertexBuffers( 0, 1, &pVertexBuffer, &stride, &offset );
		pImmediateContext->IASetIndexBuffer( pIndexBuffer, DXGI_FORMAT_R32_UINT, 0 );

		// Set primitive topology
		pImmediateContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
	}

	
    //**************************************************************************//
	// Render the back face of the cube.  We pass the index of the texture to	//
	// use in a constant buffer.												//
    //**************************************************************************//
	pImmediateContext->DrawIndexed( 6, 0, 0 );							// draw back of cube (last 6 vertices.


}

	


	
//**************************************************************************//
// Do something, up to you what.											//
//**************************************************************************//

void FlatThing3D::DoBehaviour()
{

}










