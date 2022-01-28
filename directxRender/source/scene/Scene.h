#pragma once

#include "scene/GeometryMesh.h"
#include "scene/Material.h"
#include "scene/SceneObject.h"
#include <vector>

class Scene
{
public:
	class MeshWithMaterial : public GeometryMesh
	{
	public:
		MeshWithMaterial(const GeometryMesh& mesh, Scene* parent, uint32_t materialIndex);
		Material& GetMaterial();

	private:
		uint32_t mMaterialIndex;
		Scene* mParentScene;
	};

	using MeshObjectVector = std::vector<SceneObject<MeshWithMaterial>>;

	Scene() = default;
	Scene(size_t objectsCnt);
	~Scene() = default;
	void AddMesh(const GeometryMesh& obj, uint32_t materialIndex = 0);
	void AddMaterial(const Material& material);
	void Clear();
	void LoadFromFile(const char* fileName);
	MeshObjectVector& GetMeshes();
	std::vector<Material>& GetMaterials();

private:
	MeshObjectVector mMeshes;
	std::vector<Material> mMaterials;

};

