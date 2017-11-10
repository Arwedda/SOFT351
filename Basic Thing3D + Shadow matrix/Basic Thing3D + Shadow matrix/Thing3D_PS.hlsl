//**************************************************************************//
// PIXEL Shader file for the descendent of AbstractThing3D.  It is very		//
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



cbuffer CBLighting : register (b1)
{
	float4 MaterialDiffuseColor;    // Material's diffuse color
	float4 vecLightDir;             // Light's direction in world space
	float4 LightDiffuse;			// Light's diffuse color
	float4 LightAmbientColour;      // Light's's ambient color
}


//**************************************************************************//
// Textures and Texture Samplers.  These variables can't (it seems) go in	//
// constant buffers; I think the size of a CB is too restricted.			//
//																			//
// Two shader variables; add more as required.								//
//**************************************************************************//
Texture2D ThingTexture : register( t0 );					
Texture2D TileTexture  : register( t1 );					



//**************************************************************************//
// Texture sampler.  A simple texture sampler is all we need here.			//
//**************************************************************************//
SamplerState MeshTextureSampler : register( s0 );




//**************************************************************************//
// The pixel shader.  This shader outputs the pixel's color by modulating   //
// the texture's color with diffuse material color.  As above, this is "per //
// pixel lighting, which gives better results than "per vertex lighting",	//
// but is slower.															//
//**************************************************************************//
float4 Thing3D_PS( VS_OUTPUT In ) : SV_Target
{ 
    float lighting = saturate( dot( vecLightDir, In.vecNormal ) );
	lighting = max( lighting, LightAmbientColour );

	
	// Lookup mesh texture and modulate it with diffuse
    return ThingTexture.Sample(MeshTextureSampler, In.TextureUV) * lighting;
}




//**************************************************************************//
// The pixel shader fot the flat thing. This uses a different texture, and	//
// there is no lighting.													//
//**************************************************************************//
float4 FlatThing3D_PS( VS_OUTPUT In ) : SV_Target
{ 

	// Lookup mesh texture
    return TileTexture.Sample(MeshTextureSampler, In.TextureUV) * MaterialDiffuseColor;
}

//And that's all folks.
