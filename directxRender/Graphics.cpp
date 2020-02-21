#include "Graphics.h"

#include "GfxMacros.h"
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
	CreateDeviceAndContext();
	pCam = std::make_unique<PerspectiveCamera>(1.57f, 1.0f);
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
	context->ClearRenderTargetView(renderTargetView.Get(), color);
	context->ClearDepthStencilView(depthStencilView.Get(), D3D11_CLEAR_DEPTH, 1, 0);
}

void Graphics::HandleWindowResize()
{
	RECT rect;
	GetClientRect(hWnd, &rect);
	width = static_cast<uint16_t>(rect.right - rect.left);
	height = static_cast<uint16_t>(rect.bottom - rect.top);
	Camera().SetAspect(1.0f * width / height);

	CreateSwapChain();
	CreateRenderTargetView();
	CreateDepthStencilView();

	D3D11_VIEWPORT vp = { 0 };
	vp.Width = width;
	vp.Height = height;
	vp.MaxDepth = 1;
	context->RSSetViewports(1, &vp);
	context->OMSetRenderTargets(1, renderTargetView.GetAddressOf(), depthStencilView.Get());
}

void Graphics::DrawPrimitiveMesh(const MeshPrimitive& mesh, float angle, float xOffset, float zOffset)
{
	namespace wrl = Microsoft::WRL;

	struct Color
	{
		float r;
		float g;
		float b;
		float a;
	};

	const auto& vertices = mesh.vertices;
	const auto& indices = mesh.indices;
	assert(!vertices.empty());
	assert(!indices.empty());

	D3D11_BUFFER_DESC desc = {};
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.ByteWidth = (UINT)(sizeof(vertices[0]) * vertices.size());
	desc.StructureByteStride = sizeof(vertices[0]);
	D3D11_SUBRESOURCE_DATA sd = {};
	sd.pSysMem = vertices.data();
	wrl::ComPtr<ID3D11Buffer> vertexBuffer;
	GFX_THROW_INFO(device->CreateBuffer(&desc, &sd, &vertexBuffer));
	const UINT stride = sizeof(vertices[0]);
	const UINT offset = 0;
	context->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), &stride, &offset);

	D3D11_BUFFER_DESC idesc = {};
	idesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	idesc.Usage = D3D11_USAGE_DEFAULT;
	idesc.ByteWidth = (UINT)(sizeof(indices[0]) * indices.size());
	idesc.StructureByteStride = sizeof(indices[0]);
	D3D11_SUBRESOURCE_DATA isd = {};
	isd.pSysMem = indices.data();
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
				* dx::XMMatrixTranslation(xOffset, 0, zOffset)
				* Camera().GetPerspectiveViewTransform()
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

PerspectiveCamera & Graphics::Camera()
{
	if (!pCam)
	{
		throw EXT_EXCEPT("Gfx Camera not initialized!");
	}

	return *pCam;
}

void Graphics::CreateDeviceAndContext()
{
	UINT flags = 0;
	D3D_FEATURE_LEVEL featureLevel;

#ifndef NDEBUG
	flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	GFX_THROW_INFO(D3D11CreateDevice(
		nullptr, // default adapter
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr, // no software device
		flags,
		nullptr, 0, // default feature levels
		D3D11_SDK_VERSION,
		&device,
		&featureLevel,
		&context
	));

	if (featureLevel < D3D_FEATURE_LEVEL_11_0)
	{
		throw EXT_EXCEPT("DirectX 11 is not supported");
	}
}

void Graphics::CreateSwapChain()
{
	DXGI_SWAP_CHAIN_DESC sd = {};

	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	sd.SampleDesc.Count = 1;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.BufferCount = 1;
	sd.Windowed = true;
	sd.OutputWindow = hWnd;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	wrl::ComPtr<IDXGIDevice> dDevice;
	wrl::ComPtr<IDXGIAdapter> adapter;
	wrl::ComPtr<IDXGIFactory> factory;

	GFX_THROW_INFO(device->QueryInterface(__uuidof(IDXGIDevice), &dDevice));
	GFX_THROW_INFO(dDevice->GetParent(__uuidof(IDXGIAdapter), &adapter));
	GFX_THROW_INFO(adapter->GetParent(__uuidof(IDXGIFactory), &factory));

	GFX_THROW_INFO(factory->CreateSwapChain(device.Get(), &sd, &swapChain));
}

void Graphics::CreateRenderTargetView()
{
	wrl::ComPtr<ID3D11Resource> backBuffer;
	GFX_THROW_INFO(swapChain->GetBuffer(0, __uuidof(ID3D11Resource), &backBuffer));
	GFX_THROW_INFO(device->CreateRenderTargetView(backBuffer.Get(), nullptr, &renderTargetView));
}

void Graphics::CreateDepthStencilView()
{
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
	textureDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	GFX_THROW_INFO(device->CreateTexture2D(&textureDesc, nullptr, &zBuffer));

	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
	dsvDesc.Format = textureDesc.Format;
	dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Texture2D.MipSlice = 0;
	GFX_THROW_INFO(device->CreateDepthStencilView(zBuffer.Get(), &dsvDesc, &depthStencilView));
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
