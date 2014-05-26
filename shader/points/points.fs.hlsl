struct VS_OUTPUT
{
	float4 position   : SV_Position;   // vertex position 
	float  psize      : PSIZE;		   // point size
};

float4 main(VS_OUTPUT input) : SV_Target
{
	return float4(0.0,0.0,0.0,1.0);
}