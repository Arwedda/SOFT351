//**************************************************************************//
// PIXEL SHADER for the DXUT meshes sample.									//
//																			//
// Modifications to the MS sample code is copyright of Dr Nigel Barlow,		//
// lecturer in computing, University of Plymouth, UK.						//
// email: nigel@soc.plymouth.ac.uk.											//
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
//--------------------------------------------------------------------------------------
// File: Tutorial 09 - Meshes Using DXUT Helper Classes_PS.hlsl
//
// The pixel shader file for the Tutorial 09 - Meshes Using DXUT Helper Classes sample.  
// 
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------

//**************************************************************************//
// First constant buffer, goes at position 0.  We will need to know the		//
// position in the sample that uses this code.  And the constant buffer's	//
// structure must be identical to the structure defined in the program that	//
// uses this shader.														//
//																			//
// This constant buffer changes only rarely, which is why the two buffers	//
// you can see here are kept separate.										//
//**************************************************************************//
cbuffer cbPerObject : register( b0 )
{
	float4		g_vObjectColor;
};

cbuffer cbPerFrame : register( b1 )
{
	float4   vecLight;					//Light vector.
	float4	 vecViewer;					//Vector pointing at viewer;
	float4	 lightDiffuseColour;		//Light intensities.
	float4	 lightAmbientColour;
	float4   lightSpecularColour;
	float4   materialPower;				//Only first value used.
};


//**************************************************************************//
// Textures and Texture Samplers.  These variables can't (it seems) go in	//
// constant buffers; I think the size of a CB is too restricted.			//
//																			//
// We only have one Texture2D variable here, what does that suggest about	//
// the nature or our mesh, i.e. could it be multi-textured?					//
//**************************************************************************//
Texture2D	 g_txDiffuse : register( t0 );
SamplerState g_samLinear : register( s0 );



//**************************************************************************//
// Pidel shader input structure.	The semantics (the things after the		//
// colon) look a little weird.  The semantics are used (so Microsoft tell	//
// us) used by the compiler to link shader inputs and outputs. 				//
//																			//
// For this to work, you must ensure that the vertex output structure you	//
// use in your vertex shader matches this.  Actually, in the M$ sample code //
// that wasn't the case and I added the position.  Why dod the original		//
// sample code work at all - Nigel?											//
//**************************************************************************//
struct PS_INPUT
{
	float3 vNormal		: NORMAL;
	float2 vTexcoord	: TEXCOORD0;
	float4 vPosition	: SV_POSITION;	// Nigel added position; it wasn't in M$ sample, why?
};



//**************************************************************************//
// Pixel Shader.	This one has basic lighting, however the really			//
// important part is the rexture sampler.									//
//**************************************************************************//
float4 PS_DXUTSDKMesh( PS_INPUT input ) : SV_TARGET
{
	float4 finalColour;
	float4 diffuseReflection;
	float4 ambientReflection;
	float4 specularReflection;

	float4 meshColour = g_txDiffuse.Sample(g_samLinear, input.vTexcoord);
	float3 vecNormal = normalize(input.vNormal);
	int    glossyness = materialPower.x;  // Only the first "x" value of power  //
										  // is used; same for each RGB colour.	//

	 //**********************************************************************//
	// Diffuse reflection.													//
	//**********************************************************************//
	diffuseReflection = lightDiffuseColour * meshColour *  dot(vecNormal, vecLight);


	//**********************************************************************//
	// Specular reflection.													//
	//																		//
	// We probably don't need to normalise the half vector, but we don't	//
	// get the right anseer if I do not...									//
	//**********************************************************************//
	float3 halfVector = normalize((vecLight + vecViewer) / 2.0);

	specularReflection = lightSpecularColour *  meshColour *
		pow(dot(vecNormal, halfVector), glossyness);


	//**********************************************************************//
	// Ambient reflection.													//
	//**********************************************************************//
	ambientReflection = lightAmbientColour * meshColour;


	//**********************************************************************//
	// Sum them all up.														//
	//**********************************************************************//
	finalColour = diffuseReflection + ambientReflection + specularReflection;

	finalColour = saturate(finalColour);

	return finalColour;
}