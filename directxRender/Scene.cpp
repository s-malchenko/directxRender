#include "Scene.h"

#include "ObjLoader.h"

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

void Scene::LoadObj(const char* filePath)
{
	objl::Loader loader;
	loader.LoadFile(filePath);

	for (const auto& loadedMesh : loader.LoadedMeshes)
	{
		GeometryMesh mesh;

		for (const auto& lv : loadedMesh.Vertices)
		{
			Vertex v;
			v.pos = { -lv.Position.X, lv.Position.Y, lv.Position.Z };
			v.normal = { -lv.Normal.X, lv.Normal.Y, lv.Normal.Z };

			mesh.vertices.push_back(v);
		}

		mesh.indices = loadedMesh.Indices;
		AddObject(mesh);
	}
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
