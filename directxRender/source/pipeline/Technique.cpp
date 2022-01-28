#include "Technique.h"

#pragma comment (lib, "D3DCompiler.lib")

#include "exception/GraphicsException.h"
#include <d3dcompiler.h>

namespace
{
	const std::wstring k_shaderNamePrefix = L"../../directxRender/source/shaders/";
	const std::wstring k_shaderCompiledExtension = L".cso";
	const std::wstring k_psExtension = L".ps";
	const std::wstring k_vsExtension = L".vs";
}

namespace wrl = Microsoft::WRL;

Technique::Technique(const std::string& name, const InputLayout& layout)
	: mName(name)
	, inputLayoutDesc(layout)
{
}

void Technique::Load(Microsoft::WRL::ComPtr<ID3D11Device> device)
{
	wrl::ComPtr<ID3DBlob> blob;
	const auto name = std::wstring(mName.begin(), mName.end());
	const std::wstring psFile = name + k_psExtension + k_shaderCompiledExtension;
	const std::wstring vsFile = name + k_vsExtension + k_shaderCompiledExtension;
	GFX_THROW_INFO(D3DReadFileToBlob(psFile.data(), &blob));
	GFX_THROW_INFO(device->CreatePixelShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &pShader));
	GFX_THROW_INFO(D3DReadFileToBlob(vsFile.data(), &blob));
	GFX_THROW_INFO(device->CreateVertexShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &vShader));
	GFX_THROW_INFO(device->CreateInputLayout(
		inputLayoutDesc.GetLayout(),
		inputLayoutDesc.GetSize(),
		blob->GetBufferPointer(),
		blob->GetBufferSize(),
		&inputLayout
	));
}

HRESULT CompileShader(LPCWSTR srcFile, LPCSTR entryPoint, LPCSTR profile, ID3DBlob** blob)
{
	if (!srcFile || !entryPoint || !profile || !blob)
		return E_INVALIDARG;

	*blob = nullptr;

	UINT flags = D3DCOMPILE_ENABLE_STRICTNESS;
#ifndef NDEBUG
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
	const auto name = std::wstring(mName.begin(), mName.end());
	const std::wstring psFile = k_shaderNamePrefix + name + k_psExtension;
	const std::wstring vsFile = k_shaderNamePrefix + name + k_vsExtension;
	GFX_THROW_INFO(CompileShader(psFile.data(), "main", "ps_5_0", &blob));
	GFX_THROW_INFO(device->CreatePixelShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &pShader));
	GFX_THROW_INFO(CompileShader(vsFile.data(), "main", "vs_5_0", &blob));
	GFX_THROW_INFO(device->CreateVertexShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &vShader));
	GFX_THROW_INFO(device->CreateInputLayout(
		inputLayoutDesc.GetLayout(),
		inputLayoutDesc.GetSize(),
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
