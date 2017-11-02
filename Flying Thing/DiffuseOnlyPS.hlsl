/*
	Works exactly the same as the original sample, except it doesn't calculate
	specular or ambient lighting. This is necessary for the SkyBox to display
	correctly from the inside (or the normal needs to be a point inside it).
*/
cbuffer cbPerObject : register(b0)
{
	float4		g_vObjectColor;
};

cbuffer cbPerFrame : register(b1)
{
	float3		g_vLightDir;
	float		g_fAmbient;
};

Texture2D	 g_txDiffuse : register(t0);
SamplerState g_samLinear : register(s0);

struct PS_INPUT
{
	float3 vNormal		: NORMAL;
	float2 vTexcoord	: TEXCOORD0;
	float4 vPosition	: SV_POSITION;
};

float4 PS_DXUTSDKMesh(PS_INPUT Input) : SV_TARGET
{
	float4 vDiffuse = g_txDiffuse.Sample(g_samLinear, Input.vTexcoord);

	return vDiffuse;
}