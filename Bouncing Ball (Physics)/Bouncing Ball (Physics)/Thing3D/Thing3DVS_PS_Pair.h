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

#include "..\Nig.h"
#include <d3d11.h>
#include <xnamath.h>


#ifndef Thing3DVS_PS_Pair_h   //Guards, as usual.
#define Thing3DVS_PS_Pair_h




//**************************************************************************//
// Define the input layout.  The vertex defined here MUST be consistent		//
// with the vertex shader input you use in your shader file.				//
//																			//
// Normal vectors are used by lighting.										//
//																			//
// Reserving storage like this in a header file is not good practise; I hope//
// the use of guards will sort it out.										//
//**************************************************************************//
const D3D11_INPUT_ELEMENT_DESC thing3DAvstractVertexLayoutDescription[] =
{
    { "POSITION",  0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
    { "NORMAL",    0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    { "TEXCOORD",  0, DXGI_FORMAT_R32G32_FLOAT,    0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
};





//**************************************************************************//
// Many of these member variables are public and have to be set by writing  //
// to the variable.   OO programmers would have us write many more          //
// setThisandThat(...) methods.                                             //
//**************************************************************************//
class Thing3DVS_PS_Pair
{
	//**********************************************************************//
	// Public instance variables.  OO purists will want to make these		//
	// private and write the setThis() and getThat() methods.				//
	//**********************************************************************//  
   public:	//Not quite like Java, but the same meaning.

		//**********************************************************************//
		// Constant buffers.													//
		//**********************************************************************//
		ID3D11Buffer               *pCBMatrices;
		ID3D11Buffer               *pCBLighting;


		//**********************************************************************//
		// Other buffers and pixel / vertex shaders.							//
		//**********************************************************************//
		ID3D11Buffer               *pVertexBuffer;
		ID3D11Buffer               *pIndexBuffer;
		ID3DBlob                   *pVertexShaderBuffer;		// Now kept for later.
		ID3DBlob                   *pPixelShaderBuffer;		    // Now kept for later.
		ID3D11VertexShader         *pVertexShader;
		ID3D11PixelShader          *pPixelShader;
		ID3D11SamplerState         *pSamLinear;
		ID3D11InputLayout          *pVertexLayout;

		//**************************************************************************//
		// The texture; just one here.												//
		//**************************************************************************//
		ID3D11ShaderResourceView   *pTextureResourceView;
		UINT					   textureNumber;



	public:
		//**********************************************************************//
		// First constant buffer.  The constant buffer's structure must be		//
		// identical to the structure defined in the program and its shader.	//
		//**********************************************************************//
		struct CBMatrices
		{
			XMMATRIX matWorld;                  // World matrix for object
			XMMATRIX matWorldViewProjection;    // World * View * Projection matrix
		};


		//**********************************************************************//
		// Lighting constant buffer.  it's kind-a-crazy to have this as an		//
		// attribute of the obkect, but it is....								//
		//**********************************************************************//
		struct CBLighting
		{
			XMFLOAT4 materialDiffuseColor;      // Material's  color
			XMVECTOR vecLightDir;               // Light's direction in world space
			XMFLOAT4 lightDiffuseColour;        // Light's diffuse color
			XMFLOAT4 LightAmbientColour;		// Light's's diffuse color
		};


		//**************************************************************************//
	    // Structure to define a Vertex.  Thid is linked in with the shader we use, //
		// and this won't work unless you have a shader with a posotion, normal		//
		// vector and textureUV coordinates.										//
		//**************************************************************************//
		struct VertexStruct
		{
			XMFLOAT3 pos;
			XMFLOAT3 normal;
			XMFLOAT2 texUV;
		};


	public:
		//**********************************************************************//
		// Constructor.															//
		//**********************************************************************//
		Thing3DVS_PS_Pair();





	  //*********************************************************************//
	  // Create the vertex shader.											 //
	  // The number of ways you can represent a string in C++ will drive you //
	  // nuts!																 //
	  //*********************************************************************//
	  virtual HRESULT CreatVertexShader(ID3D11Device *pRenderingDevice,
	                                    WCHAR        *fileName,
										LPCSTR        vertexShaderName);
	  
	  
	  //*********************************************************************//
	  // Create the pixel shader.											 //
	  // The number of ways you can represent a string in C++ will drive you //
	  // nuts!																 //
	  //*********************************************************************//
	  virtual HRESULT CreatPixelShader(ID3D11Device *pRenderingDevice,
		                               WCHAR        *fileName,
									   LPCSTR       pixelShaderName);


	  //********************************************************************//
	  // Create constant buffers and texture sampler.						//
 	  //********************************************************************//
	  void CreateConstandBuffersAndSampler(ID3D11Device *pRenderingDevice);


	  //********************************************************************//
	  // Set up the matrices and constant buffers.							//
	  //********************************************************************//
      void SetupShadersAndConstantbuffers(ID3D11DeviceContext 
										  *pImmediateContext);





	  //********************************************************************//
	  // Update the matrices constant buffer.								//
	  //********************************************************************//
	  void UpdateCBMatrices(ID3D11DeviceContext *pImmediateContext,
							CBMatrices          *cbMatrices);
	  




	  //********************************************************************//
	  // Update the lighting constant buffer.								//
	  //********************************************************************//
	  void UpdateCBLighting(ID3D11DeviceContext *pImmediateContext,
							CBLighting           *cbLighting);
	  




	  //********************************************************************//
	  // Set the texture.	 The texture number referrs to the pisition of	//
	  // the texture in the shader.											//
	  //********************************************************************//
	  void Thing3DVS_PS_Pair::SetTexture(ID3D11DeviceContext       *pImmediateContex,
		                                 ID3D11ShaderResourceView  *pTexture,
								         UINT                      textureNumber);



	  //********************************************************************//
	  // And finally, a couple of accessors.								//
	  //********************************************************************//
	  ID3D11VertexShader *GetVertexShader();
	  ID3D11PixelShader  *GetPixelShader();
	  

    //**********************************************************************//
	// Private and protected methods.										//
	//**********************************************************************//
	protected:
		void Init(); // Used by constructors.	


		//**********************************************************************//
		// Compile the shader file.  These files aren't pre-compiled (well,	not	//
		// here, and are compiled on he fly).  Although this one returns an		//
		// HRESULT, if there is an error in the shader it doesn't return; it	//
		// dlaplays a message box which I thought was more useful.				//
		//**********************************************************************//
		HRESULT CompileShaderFromFile(WCHAR* szFileName,	 // File Name
									  LPCSTR szEntryPoint,	 // Namee of shader
							          LPCSTR szShaderModel,  // Shader model
							          ID3DBlob** ppBlobOut); // Blob returned
		
									  
									  
	    //**********************************************************************//
		// Convert an old chrtacter (char *) string to a WCHAR * string.  There	//
		// must be something built into Visual Studio to do this for me, but I	//
		// can't find it - Nigel.												//
		//**********************************************************************//
		void CharStrToWideChar(WCHAR *dest, char *source);



};	// End of classy class definition. 
	// Must be a ";" here in C++ - weird, eh?   Nigel


#endif	//End of guard.

