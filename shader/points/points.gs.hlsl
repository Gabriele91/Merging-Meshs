//-----------------------------------------------------------------------------
// Vertex shader output structure
//-----------------------------------------------------------------------------
struct VS_OUTPUT
{
	float4 position   : SV_Position;   // vertex position 
};

struct GS_OUTPUT
{
	float4 position   : SV_Position;   // vertex position 
};

cbuffer C_UNIFORMS
{
	uniform float4x4 projection;
	uniform float4x4 view;
	uniform float4x4 model;
	uniform float2   size;
};

[maxvertexcount(4)]
void main(point VS_OUTPUT sprite[1], inout TriangleStream<GS_OUTPUT> triStream)
{
	float4 center = mul(mul(projection, view), mul(model, sprite[0].position));

	float4 p[4];
	p[0] = center + float4(size.x, size.y,   0.0f, 0.0f);
	p[1] = center + float4(size.x, -size.y,  0.0f, 0.0f);
	p[2] = center + float4(-size.x, size.y,  0.0f, 0.0f);
	p[3] = center + float4(-size.x, -size.y, 0.0f, 0.0f);

	for (int i = 0; i != 4; ++i){
		GS_OUTPUT ouput;
		ouput.position = p[i];
		triStream.Append(ouput);
	}
}


