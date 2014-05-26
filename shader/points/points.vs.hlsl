//-----------------------------------------------------------------------------
// Vertex shader output structure
//-----------------------------------------------------------------------------
struct VS_OUTPUT
{
	float4 position   : SV_Position;   // vertex position 
	float  psize      : PSIZE;		   // point size
};

struct CPU_INPUT
{
	float3 position : inPosition;
};

cbuffer C_UNIFORMS
{

	uniform float4x4 projection;
	uniform float4x4 view;
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
	output.psize    = 10.0;
	output.position = mul(mul(projection, view), mul(model, float4(input.position, 1.0)));
	return output;
}


