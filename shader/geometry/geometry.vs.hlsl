//-----------------------------------------------------------------------------
// Vertex shader output structure
//-----------------------------------------------------------------------------
struct VS_OUTPUT
{
	float4 position   : SV_Position;   
	float3 normal     : outNormal;
	float4 shadowpos  : outShadow;
};

struct CPU_INPUT
{
	float3 position : inPosition;
	float3 normal   : inNormal;
};

cbuffer C_UNIFORMS
{
	//uniform camera
	uniform float4x4 projection;
	uniform float4x4 view;
	//uniform shadow camera
	uniform float4x4 projectionSW;
	uniform float4x4 viewSW;
	//uniform model
	uniform float4x4 model;
};

//shadow bias
const float4x4 bias = {
	0.5, 0.0, 0.0, 0.0,
	0.0, 0.5, 0.0, 0.0,
	0.0, 0.0, 0.5, 0.0,
	0.5, 0.5, 0.5, 1.0 
};

//-----------------------------------------------------------------------------
// Name: main
// Type: Vertex shader                                      
// Desc: calc vertices
//-----------------------------------------------------------------------------
VS_OUTPUT main( CPU_INPUT input )
{
	VS_OUTPUT output;
	output.normal = normalize(mul(model, float4(input.normal, 0.0)).xyz);
	output.shadowpos = mul(bias, mul(projectionSW, mul(viewSW, mul(model, float4(input.position, 1.0)))));
	output.position = mul(projection, mul(view, mul(model, float4(input.position, 1.0))));
	return output;
}


