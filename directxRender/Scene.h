#pragma once

#include <vector>
#include "GeometryMesh.h"

class Scene
{
public:
	Scene() = default;
	Scene(size_t objectsCnt);
	~Scene() = default;
	void AddObject(const GeometryMesh& obj);
	void Clear();
	void LoadObj(const char* fileName);
	size_t VerticesCount() const;
	size_t IndicesCount() const;
	const std::vector<GeometryMesh>& GetObjects();
private:
	std::vector<GeometryMesh> objects;
	size_t verticesCount = 0;
	size_t indicesCount = 0;
};

