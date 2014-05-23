struct VS_OUTPUT
{
	float4 position   : SV_Position;
};

float4  main(VS_OUTPUT input) : SV_Target {

	return float4(input.position.z.xxx, 1.0);

}
