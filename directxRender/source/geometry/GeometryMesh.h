#pragma once

#include <DirectXMath.h>
#include <vector>
#include "geometry/MeshPrimitives.h"

struct Vertex
{
	DirectX::XMFLOAT3 pos;
	DirectX::XMFLOAT3 normal;
	DirectX::XMFLOAT2 texcoord;
	DirectX::XMFLOAT4 color;
};

struct GeometryMesh
{
	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;

	GeometryMesh() = default;
	GeometryMesh(const MeshPrimitive& mesh);
};
