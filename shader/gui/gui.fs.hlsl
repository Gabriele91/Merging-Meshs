
//-----------------------------------------------------------------------------
// Vertex shader output structure
//-----------------------------------------------------------------------------
struct VS_OUTPUT
{
	float4 position   : SV_Position;   // vertex position 
	float2 uv		  : TEXCOORD0;
	float4 color      : COLOR0;		   // vertex color 
};

//uniforms
cbuffer C_UNIFORMS
{
	uniform float4 scissor;
	uniform float4 viewport;
};

//shadow map
Texture2D  textureFont;
SamplerState sampleFont;


//-----------------------------------------------------------------------------
// Name: main
// Type: Pixel shader                                      
// Desc: Passes the input color through
//-----------------------------------------------------------------------------
float4 main(VS_OUTPUT input, float4 screenSpace : SV_Position) : SV_Target
{
	if (scissor.x > screenSpace.x) discard;
	if (scissor.z < screenSpace.x) discard;
	if (scissor.y > screenSpace.y) discard;
	if (scissor.w < screenSpace.y) discard;
	//scissor
	return input.color*textureFont.Sample(sampleFont, input.uv);
}