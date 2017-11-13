//**************************************************************************//
// VERTEX Shader file for the descendent of AbstractThing3D.  It is very	//
// heavily based on the shader provided in Microsoft's "SimpleSample".		//
//																			//
// This (poggled) code is copyright of Dr Nigel Barlow, lecturer in			//
// computing, University of Plymouth, UK.  email: nigel@soc.plymouth.ac.uk.	//
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
//																			//
// Poggeling ©Nigel Barlow nigel@soc.plymouth.ac.uk.						//
// "Simple Sample" ©Microsoft Corporation. All rights reserved.				//
//**************************************************************************//

#include "Thing3D_Common.hlsli"


//**************************************************************************//
// First constant buffer, goes at position 0.  We will need to know the		//
// position in the sample that uses this code.  And the constant buffer's	//
// structure must be identical to the structure defined in the program that	//
// uses this shader.														//
//**************************************************************************//
cbuffer CBMatrices: register( b0 )
{
	matrix matWorld;                  // World matrix for object
	matrix matWorldViewProjection;    // World * View * Projection matrix
}





//**************************************************************************//
// Vertex shader input structure.	The semantics (the things after the		//
// colon) look a little weird.  The semantics are used (so Microsoft tell	//
// us) used by the compiler to link shader inputs and outputs. 				//
//																			//
// For this to work, you must ensure that the vertex structure you use in	//
// any program that uses this shader is the same as below, vertex position,	//
// normal vector and the texture UV coordinates, in that order!				//
//**************************************************************************//
struct VS_INPUT
{
	float4 Position   : POSITION;	
    float3 vecNormal  : NORMAL;		
    float2 TextureUV  : TEXCOORD0;
};





//**************************************************************************//
// This shader computes standard transform and lighting.   Calculating		//
// lighting in the vertex xhader is termed "per vertex shading" and, as the //
// term suggests, performs one lighting calculation on each verted.			//
// Performing the lighting calculations in the pixel shader is more accurate//
// and gives better results, but it slower.									//
//**************************************************************************//
VS_OUTPUT Thing3D_VS(VS_INPUT input)
{
    VS_OUTPUT output;
    
	//**********************************************************************//
	// Multiply every vertex vy the WVP matrix (we do it "properly here		//
	// unlike the cubes sample.												//
	//**********************************************************************//
    output.Position = mul(input.Position, matWorldViewProjection);
    
	//**********************************************************************//
	// Whatever we do to the tiger, we must also do to its normal vector.	//
	//**********************************************************************//
    output.vecNormal = normalize(mul(input.vecNormal, (float3x3)matWorld)); 
        
    
	//**********************************************************************//
	// And finally, just copu the texture Us and Vs to the output structure	//
	//**********************************************************************//
    output.TextureUV = input.TextureUV; 
    
    return output;    
}








//And that's all folks.
