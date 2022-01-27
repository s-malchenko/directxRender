#include "scene/Scene.h"

#include "exception/ExtendedException.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>

Scene::Scene(size_t objectsCnt)
{
	m_meshes.reserve(objectsCnt);
}

void Scene::AddMesh(const SceneObject<GeometryMesh>& obj)
{
	m_meshes.emplace_back(obj);
}

void Scene::Clear()
{
	m_meshes.clear();
}

void Scene::LoadFromFile(const char* filePath)
{
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(filePath, 0);

	if (!scene)
	{
		return;
	}

	for (size_t i = 0; i < scene->mNumMeshes; ++i)
	{
		GeometryMesh mesh;

		const auto* loadedMesh = scene->mMeshes[i];

		for (size_t j = 0; j < loadedMesh->mNumVertices; ++j)
		{
			Vertex v;
			const auto lv = loadedMesh->mVertices[j];
			v.pos = { lv.x, lv.y, lv.z };

			if (loadedMesh->HasNormals())
			{
				const auto normal = loadedMesh->mNormals[j];
				v.normal = { normal.x, normal.y, normal.z };
			}

			mesh.mVertices.push_back(v);
		}

		for (size_t j = 0; j < loadedMesh->mNumFaces; ++j)
		{
			const auto face = loadedMesh->mFaces[j];
			ASSERT(face.mNumIndices == 3 || face.mNumIndices == 4, "Unsupported faces in mesh");
			mesh.mIndices.push_back(face.mIndices[0]);
			mesh.mIndices.push_back(face.mIndices[1]);
			mesh.mIndices.push_back(face.mIndices[2]);

			if (face.mNumIndices == 4)
			{
				mesh.mIndices.push_back(face.mIndices[2]);
				mesh.mIndices.push_back(face.mIndices[3]);
				mesh.mIndices.push_back(face.mIndices[0]);
			}
		}

		AddMesh({ mesh, {} });
	}
}

const Scene::MeshObjectVector& Scene::GetMeshes() const
{
	return m_meshes;
}

Scene::MeshObjectVector& Scene::GetMeshes()
{
	return m_meshes;
}
