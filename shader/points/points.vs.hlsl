//-----------------------------------------------------------------------------
// Vertex shader output structure
//-----------------------------------------------------------------------------
struct VS_OUTPUT
{
	float4 position   : SV_Position;   // vertex position 
};

struct CPU_INPUT
{
	float3 position : inPosition;
};

VS_OUTPUT main( CPU_INPUT input )
{
	VS_OUTPUT output;
	output.position = float4(input.position, 1.0);
	return output;
}


