
//-----------------------------------------------------------------------------
// Vertex shader output structure
//-----------------------------------------------------------------------------
struct VS_OUTPUT
{
	float3 normal     : NORMAL;
	float4 shadowpos  : TEXCOORD;
	float4 position   : SV_Position;
};

//shadow map
Texture2D  shadowMap;
SamplerState sampleShadow;
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
	float   shadow = 1.0;
	float	zBias = 0.001;
	//re-homogenize position after interpolation
	input.shadowpos.xyz /= input.shadowpos.w;

	//if position is not visible to the light - dont illuminate it
	//results in hard light frustum
	if (input.shadowpos.x < -1.0f || input.shadowpos.x > 1.0f ||
		input.shadowpos.y < -1.0f || input.shadowpos.y > 1.0f ||
		input.shadowpos.z <  0.0f || input.shadowpos.z > 1.0f) {
		//out shadow
		shadow = 1.0;
	}
	else{
		//transform clip space coords to texture space coords (-1:1 to 0:1)
		input.shadowpos.x = input.shadowpos.x * 0.5 + 0.5;
		input.shadowpos.y = input.shadowpos.y *-0.5 + 0.5;

		//sample shadow map - point sampler
		float shadowMapDepth = shadowMap.Sample(sampleShadow, input.shadowpos.xy).r;

		//if clip space z value greater than shadow map value then pixel is in shadow
		if (shadowMapDepth <= (input.shadowpos.z - zBias)) {
				shadow = 0.5;
		}
	}
	return diffuse*intensity*shadow;
}