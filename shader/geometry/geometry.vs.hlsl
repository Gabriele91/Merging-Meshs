//-----------------------------------------------------------------------------
// Vertex shader output structure
//-----------------------------------------------------------------------------
struct VS_OUTPUT
{
	float3 normal     : NORMAL;
	float4 shadowpos  : TEXCOORD;
	float4 position   : SV_Position;
};

struct CPU_INPUT
{
	float3 position : inPosition;
	float3 normal   : inNormal;
};

cbuffer C_UNIFORMS
{
	//uniform shadow camera
	uniform float4x4 projectionSW;
	uniform float4x4 viewSW;
	//uniform camera
	uniform float4x4 projection;
	uniform float4x4 view;
	//uniform model
	uniform float4x4 model;
};

//-----------------------------------------------------------------------------
// Name: main
// Type: Vertex shader                                      
// Desc: calc vertices
//-----------------------------------------------------------------------------
VS_OUTPUT main( CPU_INPUT input )
{
	VS_OUTPUT output;
	
	float4 modelpos  = mul(model, float4(input.position, 1.0));
	float4x4 shadowPV = mul(projectionSW, viewSW);
	float4x4 cameraPV = mul(projection, view);

	output.normal    = normalize(mul(model, float4(input.normal, 0.0)).xyz);
	output.position  = mul(cameraPV, modelpos);
	output.shadowpos = mul(shadowPV, modelpos);

	return output;
}


