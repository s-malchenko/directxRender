#include "MeshPrimitives.h"
#include <cassert>
#include <cmath>
#include <numbers>

namespace
{
	PrimitiveVertex halfUnitCircleXYVertex(float angle)
	{
		return { std::cos(angle) / 2, std::sin(angle) / 2, 0 };
	}

	std::vector<PrimitiveVertex> generateConeVertices(uint16_t baseVerticesCount)
	{
		std::vector<PrimitiveVertex> result = { { 0.0f,  0.0f,  1.0f} };

		for (uint16_t i = 0; i < baseVerticesCount; ++i)
		{
			result.push_back(halfUnitCircleXYVertex(2 * std::numbers::pi_v<float> / baseVerticesCount * i));
		}

		return result;
	}

	std::vector<uint16_t> generateConeIndices(uint16_t baseVerticesCount)
	{
		assert(baseVerticesCount % 4 == 0);
		std::vector<uint16_t> result;

		for (uint16_t i = 1; i < baseVerticesCount; ++i)
		{
			result.push_back(0);
			result.push_back(i);
			result.push_back(i + 1);
		}

		result.push_back(0);
		result.push_back(baseVerticesCount);
		result.push_back(1);

		for (uint16_t i = 1; i < baseVerticesCount; ++i)
		{
			result.push_back(1);
			result.push_back(i + 1);
			result.push_back(i);
		}

		return result;
	}
}

namespace MeshPrimitives
{
	const MeshPrimitive Cube =
	{
		// vertices
		{
			{-0.5f,  0.5f, -0.5f},
			{ 0.5f,  0.5f, -0.5f},
			{ 0.5f, -0.5f, -0.5f},
			{-0.5f, -0.5f, -0.5f},
			{-0.5f,  0.5f, 0.5f},
			{ 0.5f,  0.5f, 0.5f},
			{ 0.5f, -0.5f, 0.5f},
			{-0.5f, -0.5f, 0.5f},
		},
		// indices
		{
			0, 1, 2,
			2, 3, 0,
			1, 5, 6,
			6, 2, 1,
			5, 4, 7,
			7, 6, 5,
			4, 0, 3,
			3, 7, 4,
			4, 5, 1,
			1, 0, 4,
			3, 2, 6,
			6, 7, 3,
		}
	};

	const MeshPrimitive Cone = 
	{
		// vertices
		generateConeVertices(20),
		// indices
		generateConeIndices(20),
	};

	const MeshPrimitive Pyramid =
	{
		// vertices
		{
			{ 0.0f,  0.0f,  1.0f},
			{-0.5f, -0.5f,  0.0f},
			{ 0.5f, -0.5f,  0.0f},
			{ 0.5f,  0.5f,  0.0f},
			{-0.5f,  0.5f,  0.0f},
		},
		// indices
		{
			0, 1, 2,
			0, 2, 3,
			0, 3, 4,
			0, 4, 1,
			1, 4, 2,
			2, 4, 3,
		}
	};

	const MeshPrimitive OriginPlane =
	{
		// vertices
		{
			{ 10,  10, 0},
			{ 10, -10, 0},
			{-10, -10, 0},
			{-10,  10, 0},
		},
		// indices
		{
			0, 1, 2,
			2, 3, 0,
			0, 2, 1,
			2, 0, 3,
		}
	};
}