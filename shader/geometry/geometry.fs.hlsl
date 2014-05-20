
//-----------------------------------------------------------------------------
// Vertex shader output structure
//-----------------------------------------------------------------------------
struct VS_OUTPUT
{
	float4 position   : SV_Position;  
	float3 normal     : outNormal;    
	float4 shadowpos  : outShadow;
};

//shadow map
Texture2D  shadowMap;
SamplerState sampleShadow;
const float zBias = 0.005;

//uniforms
cbuffer C_UNIFORMS{
	float3 light; // = { 0, 2, 1 };
	float4 diffuse; // = { 1.0, 1.0, 1.0, 1.0 };
};

//uniform texture
//-----------------------------------------------------------------------------
// Name: main
// Type: Pixel shader                                      
// Desc: Passes the input color through
//-----------------------------------------------------------------------------
float4 main(VS_OUTPUT input) : SV_Target
{

	float3  lightDirection = normalize(light);
	float   intensity = max(dot(input.normal, lightDirection), 0.0);

	//shadow
	float visibility = 1.0;
	if (input.shadowpos.w > 0.0) {
		//get shadow value
		float3  sCoors = float3(input.shadowpos.xy, input.shadowpos.z - zBias) / input.shadowpos.w;
		float   sDepth = shadowMap.Sample(sampleShadow,sCoors.xy).r;
		//zbuffer
		float depth = input.shadowpos.z / input.shadowpos.w;
		//is it a pixel shadows?
		if (sDepth < depth) {
			//shadow color
			visibility = 0.5;
		}
	}

	return diffuse*intensity*visibility;
}