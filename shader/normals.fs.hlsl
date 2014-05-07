
//-----------------------------------------------------------------------------
// Vertex shader output structure
//-----------------------------------------------------------------------------
struct VS_OUTPUT
{
	float4 position   : SV_Position;   // vertex position 
	float3 normal     : outNormal;     // vertex normal 
};

/*
cbuffer C_UNIFORMS{
	float3 light = { 0, 2, 1 };
	float4 diffuse = { 1.0, 1.0, 1.0, 1.0 };
};*/

//-----------------------------------------------------------------------------
// Name: main
// Type: Pixel shader                                      
// Desc: Passes the input color through
//-----------------------------------------------------------------------------
float4 main(VS_OUTPUT input) : SV_Target
{

	float3 light = { 0, 2, 1 };
	float4 diffuse = { 1.0, 1.0, 1.0, 1.0 };

	float3  lightDirection = normalize(light);
	float   intensity = max(dot(input.normal, lightDirection), 0.0);

	return diffuse*intensity;
}