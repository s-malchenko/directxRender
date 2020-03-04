cbuffer rotMtx
{
	matrix transformation;
};

float4 main(float3 pos : Position, float4 color : Color) : SV_Position
{
	return mul(float4(pos.x, pos.y, pos.z, 1), transformation);
}
