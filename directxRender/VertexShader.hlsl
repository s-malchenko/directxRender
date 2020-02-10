struct VSOut
{
	float3 color : Color;
	float4 pos : SV_Position;
};

VSOut main(float2 pos : Position, float3 color : Color)
{
	VSOut result;
	result.pos = float4(pos.x, pos.y, 0, 1);
	result.color = color;
	return result;
}
