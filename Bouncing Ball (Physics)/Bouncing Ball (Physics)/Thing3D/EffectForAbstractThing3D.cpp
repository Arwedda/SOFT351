//**************************************************************************//
// Class to implement an effect that goes in conjunction with the class		//
// Thing3DAbstract.     It is basically just a container for an effect that //
// many instances of a child if a Thing3DAbstract may share.				//
// It is not yet finished.													//
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

#include "Nig.h"
#include "DXUT.h"
#include "DXUTmisc.h"
#include "SDKmisc.h"
#include "SDKmesh.h"
#include "EffectForAbstractThing3D.h"



//**************************************************************************//
// Convert an old chrtacter (char *) string to a WCHAR * string.  There must//
// be something built into Visual Studio to do this for me, but I can't		//
// find it - Nigel.															//
//**************************************************************************//
void charStrToWideChar(WCHAR *dest, char *source)
{
	int length = strlen(source);
	for (int i = 0; i <= length; i++)
		dest[i] = (WCHAR) source[i];
}






//**************************************************************************//
// Constructors and destructor.  We can create this effect from a file		//
// with the parmeterised constructor.										//
//**************************************************************************//

EffectForAbstractThing3D::EffectForAbstractThing3D()		//Constructors.
{
}




//**************************************************************************//
//Create effect from vertex and pixel shader file pair.						//
//**************************************************************************//
EffectForAbstractThing3D::EffectForAbstractThing3D(ID3D11Device *renderingDevice,
												   LPCTSTR fileName,
												   LPCTSTR vertexShaderName,
												   LPCTSTR pixelShaderName) 
{
	create(renderingDevice, fileName, vertexShaderName, pixelShaderName);
}




//**************************************************************************//
// Destructor.  Um, you might notice it isn't a very good destructor.		//
//**************************************************************************//
EffectForAbstractThing3D::~EffectForAbstractThing3D()	//Destructor.
{

}




//**************************************************************************//
// Create effect from vertex and pixel shader file pair.					//
//**************************************************************************//
void EffectForAbstractThing3D::create(ID3D11Device *renderingDevice,
									  LPCTSTR      fileName,
									  LPCTSTR	   vertexShaderName,
									  LPCTSTR      pixelShaderName) 

{
	HRESULT hr;
	ID3D10Blob *errors;

	DWORD dwShaderFlags = D3D10_SHADER_ENABLE_STRICTNESS;
	#if defined( DEBUG ) || defined( _DEBUG )
    // Set the D3D10_SHADER_DEBUG flag to embed debug information in the shaders.
    // Setting this flag improves the shader debugging experience, but still allows 
    // the shaders to be optimized and to run exactly the way they will run in 
    // the release configuration of this program.
    dwShaderFlags |= D3D10_SHADER_DEBUG;
    #endif

	// first, see if the effect file exists.
	WCHAR path[MAX_PATH];
    hr =  DXUTFindDXSDKMediaFileCch( path, MAX_PATH, fileName );
	V(hr);


	hr = D3DX10CreateEffectFromFile( path, NULL, NULL, "fx_4_0", 
									 dwShaderFlags, 0, renderingDevice, NULL,
                                     NULL, &pEffect, &errors, NULL );


	
	//**********************************************************************//
	// This code is simply to extract any errors that occur when compiling  //
	// the ".fx" fine and displaying them in a message box so that we can   //
	// see them.															//
	//**********************************************************************//

	if (FAILED(hr))
	{
		WCHAR errorWStr[500];
		char  *errorCharStr;
		char  errorCharStrFull[400];

		if (errors == NULL) V(hr);		//Probably file not found.

		errorCharStr = (char *) errors->GetBufferPointer();

		strcpy(errorCharStrFull, "Error compiling FX file\n\n");
		strcat(errorCharStrFull, errorCharStr);

		charStrToWideChar(errorWStr, errorCharStrFull);
		ShowMessage(errorWStr);
	}




	//**********************************************************************//
	// Obtain the te variables in the shader file that will correspond to	//
	// these lighting parameters.											//
	//**********************************************************************//
	pLightDiffuseColourInShader    = pEffect->GetVariableByName ("g_LightDiffuse")->AsVector();
	pLightDirectionInShader		   = pEffect->GetVariableByName ("g_LightDir")->AsVector();

}
