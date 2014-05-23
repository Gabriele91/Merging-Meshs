
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

static float  zBias = 0.005;
static float2 poissonDisk[16] = {
	float2(-0.94201624, -0.39906216),
	float2(0.94558609, -0.76890725),
	float2(-0.094184101, -0.92938870),
	float2(0.34495938, 0.29387760),
	float2(-0.91588581, 0.45771432),
	float2(-0.81544232, -0.87912464),
	float2(-0.38277543, 0.27676845),
	float2(0.97484398, 0.75648379),
	float2(0.44323325, -0.97511554),
	float2(0.53742981, -0.47373420),
	float2(-0.26496911, -0.41893023),
	float2(0.79197514, 0.19090188),
	float2(-0.24188840, 0.99706507),
	float2(-0.81409955, 0.91437590),
	float2(0.19984126, 0.78641367),
	float2(0.14383161, -0.14100790)
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
	float   visibility = 1.0;

	if (input.shadowpos.w > 0.0){
		//re-homogenize position after interpolation
		input.shadowpos.xyz /= input.shadowpos.w;
		//transform clip space coords to texture space coords (-1:1 to 0:1)
		input.shadowpos.x = input.shadowpos.x * 0.5 + 0.5;
		input.shadowpos.y = input.shadowpos.y *-0.5 + 0.5;

		float shadowMapDepth = 0;
		float depthbias = input.shadowpos.z - zBias;
		const float sizetexture = 1.0f / 102.4f;

		for (int i = 0; i < 16; ++i){
			//sample shadow map - point sampler
			float2 coord = input.shadowpos.xy + poissonDisk[i] * sizetexture;
			shadowMapDepth = shadowMap.SampleLevel(sampleShadow, coord, 0).r;
			//if clip space z value greater than shadow map value then pixel is in shadow
			if (shadowMapDepth < depthbias) {
				visibility = visibility - 0.022;
			}
		}
	}

	return diffuse*intensity*visibility;
}