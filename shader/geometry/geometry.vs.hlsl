//-----------------------------------------------------------------------------
// Vertex shader output structure
//-----------------------------------------------------------------------------
struct VS_OUTPUT
{
	float4 position   : SV_Position;   // vertex position 
	float3 normal     : outNormal;     // vertex normal 
};

struct CPU_INPUT
{
	float3 position : inPosition;
	float3 normal   : inNormal;
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
	output.normal = normalize( mul(model, float4(input.normal, 0.0)).xyz );
	output.position = mul( projection, mul( view, mul( model, float4(input.position, 1.0))));
	return output;
}


