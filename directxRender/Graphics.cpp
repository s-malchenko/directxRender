#include "Graphics.h"

#include "Util.h"
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <sstream>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "D3DCompiler.lib")

namespace wrl = Microsoft::WRL;
namespace dx = DirectX;

Graphics::Graphics(HWND hWnd) : hWnd(hWnd)
{
	DXGI_SWAP_CHAIN_DESC sd = { 0 };
	sd.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	sd.SampleDesc.Count = 1;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.BufferCount = 1;
	sd.OutputWindow = hWnd;
	sd.Windowed = true;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	UINT flags = 0;

#ifndef NDEBUG
	flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	GFX_THROW_INFO(
		D3D11CreateDeviceAndSwapChain(
			nullptr,
			D3D_DRIVER_TYPE_HARDWARE,
			nullptr,
			flags,
			nullptr,
			0,
			D3D11_SDK_VERSION,
			&sd,
			&swapChain,
			&device,
			nullptr,
			&context
		)
	);

	// get access to texture subresource of swap chain
	wrl::ComPtr<ID3D11Resource> backBuffer;
	GFX_THROW_INFO(swapChain->GetBuffer(0, __uuidof(ID3D11Resource), &backBuffer));
	GFX_THROW_INFO(device->CreateRenderTargetView(backBuffer.Get(), nullptr, &targetView));
	HandleWindowResize();
}

void Graphics::EndFrame()
{
#ifndef NDEBUG
	infoManager.Set();
#endif

	if (const auto hr = swapChain->Present(1, 0); FAILED(hr))
	{
		if (hr == DXGI_ERROR_DEVICE_REMOVED)
		{
			throw GFX_DEVICE_REMOVED_EXCEPT(device->GetDeviceRemovedReason());
		}

		throw GFX_EXCEPT(hr);
	}
}

void Graphics::ClearBuffer(float red, float green, float blue) noexcept
{
	const float color[] = { red, green, blue, 1 };
	context->ClearRenderTargetView(targetView.Get(), color);
}

void Graphics::HandleWindowResize()
{
	RECT rect;
	GetClientRect(hWnd, &rect);
	width = static_cast<uint16_t>(rect.right - rect.left);
	height = static_cast<uint16_t>(rect.bottom - rect.top);
	aspect = width * 1.0f / height;

	D3D11_VIEWPORT vp = { 0 };
	vp.Width = width;
	vp.Height = height;
	vp.MaxDepth = 1;
	context->RSSetViewports(1, &vp);
}

void Graphics::DrawTestTriangle(float angle, float xOffset, float yOffset)
{
	namespace wrl = Microsoft::WRL;

	struct Color
	{
		float r;
		float g;
		float b;
	};

	struct Vertex
	{
		float x;
		float y;
		Color color;
	};

	const Vertex vertices[] =
	{
		{0.0f, 1.0f, {1.0f, 0.0f, 1.0f}},
		{0.5f, -0.5f, {0.0f, 1.0f, 1.0f}},
		{-0.5f, -0.5f, {1.0f, 1.0f, 0.0f}},
		{0.7f, 0.0f, {1.0f, 1.0f, 0.0f}},
		{0.0f, -0.7f, {1.0f, 0.0f, 1.0f}},
		{-0.7f, 0.0f, {0.0f, 1.0f, 1.0f}},
	};

	D3D11_BUFFER_DESC desc = {};
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.ByteWidth = sizeof(vertices);
	desc.StructureByteStride = sizeof(Vertex);
	D3D11_SUBRESOURCE_DATA sd = {};
	sd.pSysMem = vertices;
	wrl::ComPtr<ID3D11Buffer> vertexBuffer;
	GFX_THROW_INFO(device->CreateBuffer(&desc, &sd, &vertexBuffer));
	const UINT stride = sizeof(Vertex);
	const UINT offset = 0;
	context->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), &stride, &offset);

	const uint16_t indices[] = 
	{
		0, 1, 2,
		0, 3, 1,
		1, 4, 2,
		2, 5, 0,
	};

	D3D11_BUFFER_DESC idesc = {};
	idesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	idesc.Usage = D3D11_USAGE_DEFAULT;
	idesc.ByteWidth = sizeof(indices);
	idesc.StructureByteStride = sizeof(uint16_t);
	D3D11_SUBRESOURCE_DATA isd = {};
	isd.pSysMem = indices;
	wrl::ComPtr<ID3D11Buffer> indexBuffer;
	GFX_THROW_INFO(device->CreateBuffer(&idesc, &isd, &indexBuffer));
	context->IASetIndexBuffer(indexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0);

	struct ConstantBuffer
	{
		dx::XMMATRIX transformation;
	};

	const ConstantBuffer cb = 
	{
		{
			dx::XMMatrixTranspose(
				dx::XMMatrixRotationZ(angle)
				* dx::XMMatrixScaling(1.0f / aspect, 1, 1)
				* dx::XMMatrixTranslation(xOffset, yOffset, 0)
			)
		}
	};

	D3D11_BUFFER_DESC cdesc = {};
	cdesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cdesc.Usage = D3D11_USAGE_DYNAMIC;
	cdesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cdesc.ByteWidth = sizeof(cb);
	cdesc.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA csd = {};
	csd.pSysMem = &cb;
	wrl::ComPtr<ID3D11Buffer> constantBuffer;
	GFX_THROW_INFO(device->CreateBuffer(&cdesc, &csd, &constantBuffer));
	context->VSSetConstantBuffers(0, 1, constantBuffer.GetAddressOf());

	wrl::ComPtr<ID3DBlob> blob;
	wrl::ComPtr<ID3D11PixelShader> pShader;
	GFX_THROW_INFO(D3DReadFileToBlob(L"PixelShader.cso", &blob));
	GFX_THROW_INFO(device->CreatePixelShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &pShader));
	context->PSSetShader(pShader.Get(), nullptr, 0);

	wrl::ComPtr<ID3D11VertexShader> vShader;
	GFX_THROW_INFO(D3DReadFileToBlob(L"VertexShader.cso", &blob));
	GFX_THROW_INFO(device->CreateVertexShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &vShader));
	context->VSSetShader(vShader.Get(), nullptr, 0);

	// input layout for vertices (2d position only)
	wrl::ComPtr<ID3D11InputLayout> inputLayout;
	const D3D11_INPUT_ELEMENT_DESC elementDesc[] = 
	{
		{ "Position", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "Color", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	GFX_THROW_INFO(device->CreateInputLayout(
		elementDesc,
		static_cast<UINT>(std::size(elementDesc)),
		blob->GetBufferPointer(),
		blob->GetBufferSize(),
		&inputLayout
	));

	context->IASetInputLayout(inputLayout.Get());
	context->OMSetRenderTargets(1, targetView.GetAddressOf(), nullptr);
	context->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	GFX_THROW_INFO_VOID(context->DrawIndexed(static_cast<unsigned int>(std::size(indices)), 0, 0));
}

Graphics::Exception::Exception(const char* file, int line, HRESULT hr, const std::vector<std::string>& messages)
	: HrException(file, line, hr)
{
	std::ostringstream ss;

	for (const auto& msg : messages)
	{
		ss << msg << std::endl;
	}

	info = ss.str();
}

const char* Graphics::Exception::what() const
{
	if (!info.empty())
	{
		std::ostringstream ss;
		ss << HrException::what() << std::endl << info;
		buffer = ss.str();
		return buffer.c_str();
	}

	return HrException::what();
}

const char* Graphics::Exception::GetType() const noexcept
{
	return "Graphics Exception";
}

const std::string & Graphics::Exception::GetErrorInfo() const
{
	return info;
}

Graphics::DeviceRemovedException::DeviceRemovedException(const char* file, int line, HRESULT hr, const std::vector<std::string>& messages)
	: Exception(file, line, hr, messages)
{
}

const char* Graphics::DeviceRemovedException::GetType() const noexcept
{
	return "DeviceRemoved Exception";
}
