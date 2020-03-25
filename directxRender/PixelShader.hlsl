cbuffer cbuf
{
	float4 colors[6];
};

struct VertexOut
{
	float4 pos : SV_Position;
	float3 normal : Normal;
};

float4 main(VertexOut pin) : SV_Target
{
	return float4(pin.normal, 1);
}
