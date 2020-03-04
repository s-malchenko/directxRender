#include "GeometryMesh.h"

using namespace DirectX;

GeometryMesh::GeometryMesh(const MeshPrimitive& mesh)
{
	for (const auto& v : mesh.vertices)
	{
		vertices.push_back({ XMFLOAT3(v.x, v.y, v.z), XMFLOAT4() });
	}

	for (const auto i : mesh.indices)
	{
		indices.emplace_back(i);
	}
}
