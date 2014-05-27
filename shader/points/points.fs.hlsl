struct GS_OUTPUT
{
	float4 position   : SV_Position;   // vertex position 
};

float4 main(GS_OUTPUT input) : SV_Target
{
	return float4(0.0,0.0,1.0,1.0);
}