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
	const MeshObjectVector& GetMeshes() const;
	MeshObjectVector& GetMeshes();
private:
	MeshObjectVector m_meshes;
};

