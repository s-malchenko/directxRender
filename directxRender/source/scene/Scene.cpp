#include "scene/Scene.h"

#include "exception/ExtendedException.h"
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <assimp/texture.h>

Scene::Scene(size_t objectsCnt)
{
	mMeshes.reserve(objectsCnt);
}

void Scene::AddMesh(const GeometryMesh& obj, uint32_t materialIndex)
{
	mMeshes.emplace_back(MeshWithMaterial(obj, this, materialIndex), DirectX::XMMATRIX());
}

void Scene::AddMaterial(const Material& material)
{
	mMaterials.emplace_back(material);
}

void Scene::Clear()
{
	mMeshes.clear();
}

void Scene::LoadFromFile(const char* filePath)
{
	Assimp::Importer importer;
	const aiScene* scene =
		importer.ReadFile(filePath, aiProcess_GenSmoothNormals | aiProcess_JoinIdenticalVertices);

	if (!scene)
	{
		return;
	}

	for (size_t i = 0; i < scene->mNumMaterials; ++i)
	{
		const auto* loadedMaterial = scene->mMaterials[i];

		if (loadedMaterial->GetTextureCount(aiTextureType_DIFFUSE) > 0)
		{
			aiString path;

			loadedMaterial->GetTexture(
				aiTextureType_DIFFUSE,
				0,
				&path);
			path.C_Str();
		}
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

			if (loadedMesh->HasTextureCoords(0))
			{
				v.texcoord = { loadedMesh->mTextureCoords[0]->x, loadedMesh->mTextureCoords[0]->y };
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

		AddMesh(mesh, loadedMesh->mMaterialIndex);
	}
}

Scene::MeshObjectVector& Scene::GetMeshes()
{
	return mMeshes;
}

std::vector<Material>& Scene::GetMaterials()
{
	return mMaterials;
}

Scene::MeshWithMaterial::MeshWithMaterial(const GeometryMesh& mesh, Scene* parent, uint32_t materialIndex)
	: GeometryMesh(mesh), mParentScene(parent), mMaterialIndex(materialIndex)
{
}

Material& Scene::MeshWithMaterial::GetMaterial()
{
	return mParentScene->GetMaterials()[mMaterialIndex];
}
