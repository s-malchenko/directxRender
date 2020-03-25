#include "GeometryMesh.h"

using namespace DirectX;

GeometryMesh::GeometryMesh(const MeshPrimitive& mesh)
{
	for (const auto& v : mesh.vertices)
	{
		Vertex vertex;
		vertex.pos = XMFLOAT3(v.x, v.y, v.z);
		vertices.push_back(vertex);
	}

	for (const auto i : mesh.indices)
	{
		indices.emplace_back(i);
	}
}
