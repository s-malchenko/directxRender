#pragma once

#include "scene/MeshPrimitives.h"

#include "renderSystem/EasyD3D11.h"
#include <DirectXMath.h>
#include <vector>
#include <wrl.h>

struct Vertex
{
	DirectX::XMFLOAT3 pos;
	DirectX::XMFLOAT3 normal;
	DirectX::XMFLOAT2 texcoord;
	DirectX::XMFLOAT4 color;
};

class GeometryMesh
{
public:
	GeometryMesh() = default;
	GeometryMesh(const GeometryMesh&) = default;
	GeometryMesh(const MeshPrimitive& mesh);

	void InitializeBuffers(Microsoft::WRL::ComPtr<ID3D11Device> device);
	bool IsInitialized() const;
	void ClearBuffers();
	void SetBuffers(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context);
	unsigned int GetIndexCount() const;

	std::vector<Vertex> mVertices;
	std::vector<unsigned int> mIndices;
private:

	Microsoft::WRL::ComPtr<ID3D11Buffer> mVertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> mIndexBuffer;
};
