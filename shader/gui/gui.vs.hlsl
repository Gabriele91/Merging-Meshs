//-----------------------------------------------------------------------------
// Vertex shader output structure
//-----------------------------------------------------------------------------
struct VS_OUTPUT
{
	float4 position   : SV_Position;   // vertex position 
	float2 uv		  : TEXCOORD0;	   // vertex uv 
	float4 color      : COLOR0;		   // vertex color 
};

struct CPU_INPUT
{
	float2 position : inPosition;
	float2 uv       : inCoords;
	float4 color	: inColor;
};

cbuffer C_UNIFORMS
{
	uniform float4x4 projection;
};

//-----------------------------------------------------------------------------
// Name: main
// Type: Vertex shader                                      
// Desc: calc vertices
//-----------------------------------------------------------------------------
VS_OUTPUT main( CPU_INPUT input )
{
	VS_OUTPUT output;
	output.color	= input.color;
	output.uv       = input.uv;
	output.position = mul( projection, float4(input.position, 0.0, 1.0) );
	return output;
}


