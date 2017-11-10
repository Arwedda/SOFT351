//**************************************************************************//
// Common structures for the Thing3D vertex and pixel shaders.				// 
//																			//
// This  code is copyright of Dr Nigel Barlow, lecturer in	computing,		//
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
//																			//
// This isn't M$ sample code; I done it all myself!							//
// ©Nigel Barlow nigel@soc.plymouth.ac.uk.									//
//**************************************************************************//


//**************************************************************************//
// Vertex shader output structure.	This is also the input to the pixel		//
// shader.																	//
//**************************************************************************//
struct VS_OUTPUT
{
 	float3 vecNormal	: NORMAL;
	float2 TextureUV	: TEXCOORD0;
	float4 Position		: SV_POSITION;	
};


//And that's all folks.
