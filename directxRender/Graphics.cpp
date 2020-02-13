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
	context->ClearDepthStencilView(depthStencilView.Get(), D3D11_CLEAR_DEPTH, 1, 0);
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

	D3D11_DEPTH_STENCIL_DESC ds = {};
	ds.DepthEnable = true;
	ds.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	ds.DepthFunc = D3D11_COMPARISON_LESS;
	wrl::ComPtr<ID3D11DepthStencilState> dsState;
	GFX_THROW_INFO(device->CreateDepthStencilState(&ds, &dsState));
	context->OMSetDepthStencilState(dsState.Get(), 1);

	wrl::ComPtr<ID3D11Texture2D> zBuffer;
	D3D11_TEXTURE2D_DESC textureDesc = {};
	textureDesc.Width = width;
	textureDesc.Height = height;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_D32_FLOAT;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	GFX_THROW_INFO(device->CreateTexture2D(&textureDesc, nullptr, &zBuffer));

	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
	dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Texture2D.MipSlice = 0;
	GFX_THROW_INFO(device->CreateDepthStencilView(zBuffer.Get(), &dsvDesc, &depthStencilView));

	context->OMSetRenderTargets(1, targetView.GetAddressOf(), depthStencilView.Get());
}

void Graphics::DrawTestCube(float angle, float xOffset, float zOffset)
{
	namespace wrl = Microsoft::WRL;

	struct Color
	{
		float r;
		float g;
		float b;
		float a;
	};

	struct Vertex
	{
		float x;
		float y;
		float z;
	};

	const Vertex vertices[] =
	{
		{-0.5f,  0.5f, -0.5f},
		{ 0.5f,  0.5f, -0.5f},
		{ 0.5f, -0.5f, -0.5f},
		{-0.5f, -0.5f, -0.5f},
		{-0.5f,  0.5f, 0.5f},
		{ 0.5f,  0.5f, 0.5f},
		{ 0.5f, -0.5f, 0.5f},
		{-0.5f, -0.5f, 0.5f},
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
		2, 3, 0,
		1, 5, 6,
		6, 2, 1,
		5, 4, 7,
		7, 6, 5,
		4, 0, 3,
		3, 7, 4,
		4, 5, 1,
		1, 0, 4,
		3, 2, 6,
		6, 7, 3,
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
				dx::XMMatrixRotationRollPitchYaw(angle, angle / 1.3f, angle * 1.3f)
				* dx::XMMatrixTranslation(xOffset, 0, 4 + zOffset)
				* dx::XMMatrixPerspectiveFovLH(1, aspect, 1, 7)
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

	const Color faceColors[] =
	{
		{1.0f, 0.0f, 1.0f, 1.0f},
		{0.0f, 1.0f, 1.0f, 1.0f},
		{1.0f, 1.0f, 0.0f, 1.0f},
		{1.0f, 0.0f, 0.0f, 1.0f},
		{0.0f, 1.0f, 0.0f, 1.0f},
		{0.0f, 0.0f, 1.0f, 1.0f},
	};

	cdesc = {};
	cdesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cdesc.Usage = D3D11_USAGE_DYNAMIC;
	cdesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cdesc.ByteWidth = sizeof(faceColors);
	cdesc.StructureByteStride = 0;
	csd = {};
	csd.pSysMem = faceColors;
	wrl::ComPtr<ID3D11Buffer> constantBufferColors;
	GFX_THROW_INFO(device->CreateBuffer(&cdesc, &csd, &constantBufferColors));
	context->PSSetConstantBuffers(0, 1, constantBufferColors.GetAddressOf());

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
		{ "Position", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	GFX_THROW_INFO(device->CreateInputLayout(
		elementDesc,
		static_cast<UINT>(std::size(elementDesc)),
		blob->GetBufferPointer(),
		blob->GetBufferSize(),
		&inputLayout
	));

	context->IASetInputLayout(inputLayout.Get());
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
