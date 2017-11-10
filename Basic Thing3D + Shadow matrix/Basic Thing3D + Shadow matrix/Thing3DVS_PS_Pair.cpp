//**************************************************************************//
//  Class to implement a simple vertex and picel shader pair for a Thing3D  //
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


#include "Thing3DVS_PS_Pair.h"
#include "DXUT.h"
#include "DXUTgui.h"
#include "DXUTmisc.h"
#include "DXUTSettingsDlg.h"
#include "SDKmisc.h"
#include "SDKmesh.h"
#include <string.h>
#include <wchar.h>



//**************************************************************************//
// Constructor.																//
//**************************************************************************//
Thing3DVS_PS_Pair::Thing3DVS_PS_Pair()
{
}




//**************************************************************************//
// Create the vertex shader.												//
// The number of ways you can represent a string in C++ will drive you nuts!//
//**************************************************************************//
HRESULT Thing3DVS_PS_Pair::CreatVertexShader(ID3D11Device *pRenderingDevice,
											 WCHAR        *fileName,
										     LPCSTR       vertexShaderName)
{
	HRESULT hr = 0;	// needed for V macro.

	//**********************************************************************//
    // Compile the pixel and vertex shaders.  If your computer doesn't		//
	// support shader model 5, try shader model 4.  There is nothing we are //
	// using here that needs shader model 5.								//
	//**********************************************************************..
	V_RETURN( CompileShaderFromFile( fileName, vertexShaderName, "vs_5_0", &pVertexShaderBuffer ) );
   	V_RETURN( pRenderingDevice->CreateVertexShader( pVertexShaderBuffer->GetBufferPointer(),
                                                    pVertexShaderBuffer->GetBufferSize(), NULL, &pVertexShader ) );
    DXUT_SetDebugName( pVertexShader, "VS_Thing3DVS_PS_Pair" );
 

	//**********************************************************************//
	// Create the vertex layout.											//
	//**********************************************************************//
	V_RETURN( pRenderingDevice->CreateInputLayout( thing3DAvstractVertexLayoutDescription, 
		                                          ARRAYSIZE( thing3DAvstractVertexLayoutDescription ), 
										   	      pVertexShaderBuffer->GetBufferPointer(),
                                                  pVertexShaderBuffer->GetBufferSize(), 
											      &pVertexLayout ) );
    DXUT_SetDebugName( pVertexLayout, "Thing3DVS_PS_Pair" );




	return S_OK;
}



//**************************************************************************//
// Create the pixel shader.													//
// The number of ways you can represent a string in C++ will drive you nuts!//
//**************************************************************************//
HRESULT Thing3DVS_PS_Pair::CreatPixelShader(ID3D11Device *pRenderingDevice,
											WCHAR        *fileName,
									        LPCSTR       pixelShaderName)
{
	HRESULT hr = 0;	// needed for V macro.

    V_RETURN( CompileShaderFromFile(fileName , pixelShaderName, "ps_5_0", &pPixelShaderBuffer ) );
	V_RETURN( pRenderingDevice->CreatePixelShader( pPixelShaderBuffer->GetBufferPointer(),
                                             pPixelShaderBuffer->GetBufferSize(), NULL, &pPixelShader ) );
    DXUT_SetDebugName( pPixelShader, "PS_Thing3DVS_PS_Pair" );

	// Clean up, like Nigel often forgets to do.
 	return S_OK;
}







//**************************************************************************//
// Set up the matrices and constant buffers.								//
//**************************************************************************//
void Thing3DVS_PS_Pair::SetupShadersAndConstantbuffers(ID3D11DeviceContext 
										               *pImmediateContext)
{
	pImmediateContext->VSSetShader( pVertexShader, NULL, 0 );
    pImmediateContext->PSSetShader( pPixelShader,  NULL, 0 );

	pImmediateContext->VSSetConstantBuffers( 0, 1, &pCBMatrices );
	pImmediateContext->PSSetConstantBuffers(0, 1, &pCBLighting);
    pImmediateContext->PSSetSamplers( 0, 1, &pSamLinear );


}




 //**************************************************************************//
 // Create constant buffers and texture sampler.							//
 //*************************************************************************//
void Thing3DVS_PS_Pair::CreateConstandBuffersAndSampler(ID3D11Device        
														*pRenderingDevice)
{	  
	HRESULT hr = 0;	// needed for V macro.

	//**************************************************************************//
   	// Create a sampler state
	//**************************************************************************//
    D3D11_SAMPLER_DESC SamDesc;
    SamDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    SamDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    SamDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    SamDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    SamDesc.MipLODBias = 0.0f;
    SamDesc.MaxAnisotropy = 1;
    SamDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
    SamDesc.BorderColor[0] = SamDesc.BorderColor[1] = SamDesc.BorderColor[2] = SamDesc.BorderColor[3] = 0;
    SamDesc.MinLOD = 0;
    SamDesc.MaxLOD = D3D11_FLOAT32_MAX;
	V(pRenderingDevice->CreateSamplerState( &SamDesc, &pSamLinear));
    DXUT_SetDebugName( pSamLinear, "Thing3D Abstract" );

    
	//**************************************************************************//
	// Create the 2 constant bufers, using the same buffer descriptor to create //
	// all three.																//
	//**************************************************************************//
    D3D11_BUFFER_DESC Desc;
 	ZeroMemory( &Desc, sizeof(Desc) );
    Desc.Usage = D3D11_USAGE_DEFAULT;
    Desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    Desc.CPUAccessFlags = 0;
    Desc.MiscFlags = 0;
	
    Desc.ByteWidth = sizeof( CBMatrices );
    V( pRenderingDevice->CreateBuffer( &Desc, NULL, &pCBMatrices ) );
    DXUT_SetDebugName( pCBMatrices, "CBMatrices" );

	Desc.ByteWidth = sizeof( CBLighting );
    V( pRenderingDevice->CreateBuffer( &Desc, NULL, &pCBLighting ) );
    DXUT_SetDebugName( pCBLighting, "CBLighting" );
}





//**************************************************************************//
// Update the matrices constant buffer.  Note, we pass the matric by value	//
// rather than as a reference, as copying the entire matrix is safer, in	//
// case it was created as a local variable in the module that uses this.	//
//**************************************************************************//
void Thing3DVS_PS_Pair::UpdateCBMatrices(ID3D11DeviceContext *pImmediateContext,
							             CBMatrices          *cbMatrices)
{

	pImmediateContext->UpdateSubresource( pCBMatrices, 0, NULL, cbMatrices, 0, 0 );
	pImmediateContext->VSSetConstantBuffers( 0, 1, &pCBMatrices );
}





//**************************************************************************//
// Set the texture.	 The texture number referrs to the pisition of the		//
// texture in the shader.													//
//**************************************************************************//
void Thing3DVS_PS_Pair::SetTexture(ID3D11DeviceContext       *pImmediateContex,
								   ID3D11ShaderResourceView  *pTexture,
								   UINT                      textureNumber)
{
	pTextureResourceView = pTexture;
	this->textureNumber  = textureNumber;
	pImmediateContex->PSSetShaderResources( 1, 1, &pTextureResourceView );

}



//**************************************************************************//
// And finally, a couple of accessors.										//
//**************************************************************************//
ID3D11VertexShader *Thing3DVS_PS_Pair::GetVertexShader()
{
	return pVertexShader;
}
ID3D11PixelShader *Thing3DVS_PS_Pair::GetPixelShader()
{
	return pPixelShader;
}



//**************************************************************************//
// Update the lighting constant buffer.  Note, we pass the matric by value	//
// rather than as a reference, as copying the entire matrix is safer, in	//
// case it was created as a local variable in the module that uses this.	//
//**************************************************************************//
void Thing3DVS_PS_Pair::UpdateCBLighting(ID3D11DeviceContext *pImmediateContext,
										 CBLighting          *cbLighting)
{
	pImmediateContext->UpdateSubresource( pCBLighting, 0, NULL, cbLighting, 0, 0 );
	pImmediateContext->PSSetConstantBuffers( 1, 1, &pCBLighting );
}




//**************************************************************************//
// Compile the shader file.  These files aren't pre-compiled (well, not		//
// here, and are compiled on he fly).  Although this one returns an HRESULT,//
// if there is an error in the shader it doesn't return; it dlaplays a		//
// message box which I thought was more useful.								//
//**************************************************************************//
HRESULT Thing3DVS_PS_Pair::CompileShaderFromFile(WCHAR* szFileName,	 // File Name
											  LPCSTR szEntryPoint,	 // Namee of shader
							                  LPCSTR szShaderModel,  // Shader model
							                  ID3DBlob** ppBlobOut ) // Blob returned
{
    HRESULT hr = S_OK;

    // find the file
    WCHAR str[MAX_PATH];
    V_RETURN( DXUTFindDXSDKMediaFileCch( str, MAX_PATH, szFileName ) );

    DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
    // Set the D3DCOMPILE_DEBUG flag to embed debug information in the shaders.
    // Setting this flag improves the shader debugging experience, but still allows 
    // the shaders to be optimized and to run exactly the way they will run in 
    // the release configuration of this program.
    dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif

    ID3DBlob* pErrorBlob;
    hr = D3DX11CompileFromFile( str, NULL, NULL, szEntryPoint, szShaderModel, 
        dwShaderFlags, 0, NULL, ppBlobOut, &pErrorBlob, NULL );
    if( FAILED(hr) )
    {
		WCHAR errorCharsW[1000];
        if( pErrorBlob != NULL )
		{
			CharStrToWideChar(errorCharsW, (char *)pErrorBlob->GetBufferPointer());
            MessageBox( 0, errorCharsW, L"Shader file compile error", 0 );
			return E_FAIL;
		}
        if( pErrorBlob ) pErrorBlob->Release();
        return hr;
    }
    SAFE_RELEASE( pErrorBlob );

    return S_OK;
}





//**************************************************************************//
// Convert an old chrtacter (char *) string to a WCHAR * string.  There must//
// be something built into Visual Studio to do this for me, but I can't		//
// find it - Nigel.															//
//**************************************************************************//
void Thing3DVS_PS_Pair::CharStrToWideChar(WCHAR *dest, char *source)
{
	int length = strlen(source);
	for (int i = 0; i <= length; i++)
		dest[i] = (WCHAR) source[i];
}


