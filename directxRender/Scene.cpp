#include "Scene.h"

Scene::Scene(size_t objectsCnt)
{
	objects.reserve(objectsCnt);
}

void Scene::AddObject(const GeometryMesh& obj)
{
	verticesCount += obj.vertices.size();
	indicesCount += obj.indices.size();
	objects.emplace_back(obj);
}

void Scene::Clear()
{
	objects.clear();
	verticesCount = 0;
	indicesCount = 0;
}

size_t Scene::VerticesCount() const
{
	return verticesCount;
}

size_t Scene::IndicesCount() const
{
	return indicesCount;
}

const std::vector<GeometryMesh>& Scene::GetObjects()
{
	return objects;
}
