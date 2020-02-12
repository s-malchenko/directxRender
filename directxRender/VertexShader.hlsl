struct VSOut
{
	float3 color : Color;
	float4 pos : SV_Position;
};

cbuffer rotMtx
{
	matrix transformation;
};

VSOut main(float3 pos : Position, float3 color : Color)
{
	VSOut result;
	result.pos = mul(float4(pos.x, pos.y, pos.z, 1), transformation);
	result.color = color;
	return result;
}
