cbuffer rotMtx
{
	matrix transformation;
};

struct VertexIn
{
	float3 pos : Position;
	float3 normal : Normal;
	float4 color : Color;
	float2 texcoord : Texcoord;
};

struct VertexOut
{
	float4 pos : SV_Position;
	float3 normal : Normal;
};

VertexOut main(VertexIn vin)
{
	VertexOut vOut;
	vOut.pos = mul(float4(vin.pos, 1), transformation);
	vOut.normal = mul(float4(vin.normal, 0), transformation);
	return vOut;
}
