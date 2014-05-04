//-----------------------------------------------------------------------------
// Vertex shader output structure
//-----------------------------------------------------------------------------
struct VS_OUTPUT
{
	float4 position   : SV_Position;   // vertex position 
	float4 diffuse    : COLOR0;        // vertex diffuse color
};

struct CPU_INPUT
{
	float3 position : inPosition;
	float3 color    : inColor;
};

cbuffer C_UNIFORMS
{
	float4 constVec4;
	float4x4 constMat4;
	float constAlpha;
};

//-----------------------------------------------------------------------------
// Name: main
// Type: Vertex shader                                      
// Desc: calc vertices
//-----------------------------------------------------------------------------
VS_OUTPUT main( CPU_INPUT input )
{
	VS_OUTPUT output;
	output.position = mul( constMat4, (constVec4 + float4(input.position, 1.0)) );
	output.diffuse = float4(input.color, 1.0)*constAlpha;
	return output;
}


