#pragma once

#include "scene/GeometryMesh.h"
#include "scene/SceneObject.h"
#include <vector>

class Scene
{
public:
	using MeshObjectVector = std::vector<SceneObject<GeometryMesh>>;

	Scene() = default;
	Scene(size_t objectsCnt);
	~Scene() = default;
	void AddMesh(const SceneObject<GeometryMesh>& obj);
	void Clear();
	void LoadFromFile(const char* fileName);
	size_t VerticesCount() const;
	size_t IndicesCount() const;
	const MeshObjectVector& GetMeshes() const;
private:
	MeshObjectVector m_meshes;
	size_t verticesCount = 0;
	size_t indicesCount = 0;
};

