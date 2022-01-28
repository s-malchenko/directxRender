#include "GeometryMesh.h"

#include "exception/GraphicsException.h"

using namespace DirectX;

GeometryMesh::GeometryMesh(const MeshPrimitive& mesh)
{
	for (const auto& v : mesh.vertices)
	{
		Vertex vertex;
		vertex.pos = XMFLOAT3(v.x, v.y, v.z);
		mVertices.push_back(vertex);
	}

	for (const auto i : mesh.indices)
	{
		mIndices.emplace_back(i);
	}
}

void GeometryMesh::InitializeBuffers(Microsoft::WRL::ComPtr<ID3D11Device> device)
{
	D3D11_BUFFER_DESC desc = {};
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.ByteWidth = static_cast<UINT>(sizeof(mVertices[0]) * mVertices.size());
	desc.StructureByteStride = sizeof(mVertices[0]);
	D3D11_SUBRESOURCE_DATA sd = {};
	sd.pSysMem = mVertices.data();
	GFX_THROW_INFO(device->CreateBuffer(&desc, &sd, &mVertexBuffer));

	D3D11_BUFFER_DESC idesc = {};
	idesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	idesc.Usage = D3D11_USAGE_DEFAULT;
	idesc.ByteWidth = (UINT)(sizeof(mIndices[0]) * mIndices.size());
	idesc.StructureByteStride = sizeof(mIndices[0]);
	D3D11_SUBRESOURCE_DATA isd = {};
	isd.pSysMem = mIndices.data();
	GFX_THROW_INFO(device->CreateBuffer(&idesc, &isd, &mIndexBuffer));
}

bool GeometryMesh::IsInitialized() const
{
	return mVertexBuffer && mIndexBuffer;
}

void GeometryMesh::ClearBuffers()
{
	mVertexBuffer.Reset();
	mIndexBuffer.Reset();
}

void GeometryMesh::SetBuffers(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context)
{
	const UINT stride = sizeof(mVertices[0]);
	const UINT offset = 0;
	context->IASetVertexBuffers(0, 1, mVertexBuffer.GetAddressOf(), &stride, &offset);
	context->IASetIndexBuffer(mIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
}

unsigned int GeometryMesh::GetIndexCount() const
{
	return static_cast<unsigned int>(mIndices.size());
}
