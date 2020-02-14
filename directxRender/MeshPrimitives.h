#pragma once

#include <vector>

struct PrimitiveVertex
{
	float x;
	float y;
	float z;
};

struct MeshPrimitive
{
	std::vector<PrimitiveVertex> vertices;
	std::vector<uint16_t> indices;
};

namespace MeshPrimitives
{
	extern const MeshPrimitive Cube;
	extern const MeshPrimitive Cone;
	extern const MeshPrimitive Pyramid;
}
