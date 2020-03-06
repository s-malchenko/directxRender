cbuffer cbuf
{
	float4 colors[6];
};

float4 main(uint tid : SV_PrimitiveID) : SV_Target
{
	return 0.5 * colors[tid % 5];
}
