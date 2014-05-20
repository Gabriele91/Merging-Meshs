struct VS_OUTPUT
{
	float4 position   : SV_Position;   // vertex position 
};

//-----------------------------------------------------------------------------
// Name: main
// Type: Pixel shader                                      
// Desc: Passes the input color through
//-----------------------------------------------------------------------------
//float4 main(VS_OUTPUT input) : SV_Target
//{
//	return float4(1.0,1.0,1.0,1.0);
//}
void main(VS_OUTPUT input) {}