#include "Technique.h"

#pragma comment (lib, "D3DCompiler.lib")

#include "GraphicsException.h"
#include <d3dcompiler.h>

namespace wrl = Microsoft::WRL;

Technique::Technique(ShaderNames shaders) : names(shaders)
{
}

void Technique::Load(Microsoft::WRL::ComPtr<ID3D11Device> device)
{
	wrl::ComPtr<ID3DBlob> blob;
	const auto psFile = names.pixelShader + L".cso";
	const auto vsFile = names.vertexShader + L".cso";
	GFX_THROW_INFO(D3DReadFileToBlob(psFile.data(), &blob));
	GFX_THROW_INFO(device->CreatePixelShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &pShader));
	GFX_THROW_INFO(D3DReadFileToBlob(vsFile.data(), &blob));
	GFX_THROW_INFO(device->CreateVertexShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &vShader));

	const D3D11_INPUT_ELEMENT_DESC elementDesc[] =
	{
		{ "Position", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "Color", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	GFX_THROW_INFO(device->CreateInputLayout(
		elementDesc,
		static_cast<UINT>(std::size(elementDesc)),
		blob->GetBufferPointer(),
		blob->GetBufferSize(),
		&inputLayout
	));
}

HRESULT CompileShader(_In_ LPCWSTR srcFile, _In_ LPCSTR entryPoint, _In_ LPCSTR profile, _Outptr_ ID3DBlob** blob)
{
	if (!srcFile || !entryPoint || !profile || !blob)
		return E_INVALIDARG;

	*blob = nullptr;

	UINT flags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
	flags |= D3DCOMPILE_DEBUG;
#endif

	const D3D_SHADER_MACRO defines[] =
	{
		"EXAMPLE_DEFINE", "1",
		NULL, NULL
	};

	ID3DBlob* shaderBlob = nullptr;
	ID3DBlob* errorBlob = nullptr;
	HRESULT hr = D3DCompileFromFile(srcFile, defines, D3D_COMPILE_STANDARD_FILE_INCLUDE,
		entryPoint, profile,
		flags, 0, &shaderBlob, &errorBlob);
	if (FAILED(hr))
	{
		if (errorBlob)
		{
			OutputDebugStringA((char*)errorBlob->GetBufferPointer());
			errorBlob->Release();
		}

		if (shaderBlob)
			shaderBlob->Release();

		return hr;
	}

	*blob = shaderBlob;

	return hr;
}

void Technique::Rebuild(Microsoft::WRL::ComPtr<ID3D11Device> device)
{
	wrl::ComPtr<ID3DBlob> blob;
	const auto psFile = L"..//..//directxRender//" + names.pixelShader + L".hlsl";
	const auto vsFile = L"..//..//directxRender//" + names.vertexShader + L".hlsl";
	CompileShader(psFile.data(), "main", "ps_5_0", &blob);
	GFX_THROW_INFO(device->CreatePixelShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &pShader));
	CompileShader(vsFile.data(), "main", "vs_5_0", &blob);
	GFX_THROW_INFO(device->CreateVertexShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &vShader));

	const D3D11_INPUT_ELEMENT_DESC elementDesc[] =
	{
		{ "Position", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "Color", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	GFX_THROW_INFO(device->CreateInputLayout(
		elementDesc,
		static_cast<UINT>(std::size(elementDesc)),
		blob->GetBufferPointer(),
		blob->GetBufferSize(),
		&inputLayout
	));
}



void Technique::Bind(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context) const
{
	context->PSSetShader(pShader.Get(), nullptr, 0);
	context->VSSetShader(vShader.Get(), nullptr, 0);
	context->IASetInputLayout(inputLayout.Get());
	context->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}
