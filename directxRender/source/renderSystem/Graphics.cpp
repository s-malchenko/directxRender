#include "renderSystem/Graphics.h"

#include "exception/GraphicsException.h"
#include "pipeline/TechniqueFactory.h"
#include "utility/Util.h"

#include <algorithm>
#include <assimp/Importer.hpp>
#include <DirectXMath.h>
#include <imgui/backends/imgui_impl_dx11.h>
#include <iterator>
#include <vector>

#pragma comment(lib, "d3d11.lib")

namespace wrl = Microsoft::WRL;
namespace dx = DirectX;

Graphics::Graphics(HWND hWnd) : hWnd(hWnd)
{
	CreateDeviceAndContext();
	RenewSize();
	CreateSwapChain();
	CreateRenderTargetView();
	CreateDepthStencilView();
	SetViewport();
	mCurrentTechnique.emplace(TechniqueFactory::CreateDefaultTechnique());
	mCurrentTechnique->Load(device);
	mCurrentTechnique->Bind(context);
	ImGui_ImplDX11_Init(device.Get(), context.Get());
}

Graphics::~Graphics()
{
	ImGui_ImplDX11_Shutdown();
}

void Graphics::NewFrameUI()
{
	ImGui_ImplDX11_NewFrame();
}

void Graphics::EndFrame()
{
#ifndef NDEBUG
	GraphicsException::infoManager.Set();
#endif

	if (const auto hr = swapChain->Present(0, 0); FAILED(hr))
	{
		if (hr == DXGI_ERROR_DEVICE_REMOVED)
		{
			throw GFX_DEVICE_REMOVED_EXCEPT(device->GetDeviceRemovedReason());
		}

		throw GFX_EXCEPT(hr);
	}
}

void Graphics::ClearBuffer() noexcept
{
	static const float color[] = { 0.7f, 0.7f, 0.7f, 1 };
	context->ClearRenderTargetView(renderTargetView.Get(), color);
	context->ClearDepthStencilView(depthStencilView.Get(), D3D11_CLEAR_DEPTH, 1, 0);
}

void Graphics::HandleWindowResize()
{
	if (!mResizeNeeded)
	{
		return;
	}

	mResizeNeeded = false;
	RenewSize();

	if (width == 0 || height == 0)
	{
		return;
	}

	renderTargetView = nullptr;
	depthStencilView = nullptr;
	GFX_THROW_INFO(swapChain->ResizeBuffers(0, width, height, DXGI_FORMAT_UNKNOWN, 0));
	CreateRenderTargetView();
	CreateDepthStencilView();
	SetViewport();
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
	constexpr DXGI_FORMAT textureFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
	
	DXGI_SWAP_CHAIN_DESC sd = {};
	GFX_THROW_INFO(device->CheckMultisampleQualityLevels(textureFormat, sampleCount, &msaaQuality));

	sd.BufferDesc.Format = textureFormat;
	sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	sd.SampleDesc.Count = msaaQuality > 0 ? sampleCount : 1;
	sd.SampleDesc.Quality = msaaQuality > 0 ? msaaQuality - 1 : 0;
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
	wrl::ComPtr<ID3D11Texture2D> backBuffer;
	GFX_THROW_INFO(swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), &backBuffer));
	D3D11_TEXTURE2D_DESC bufferDesc;
	backBuffer->GetDesc(&bufferDesc);
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
	textureDesc.SampleDesc.Count = msaaQuality > 0 ? sampleCount : 1;
	textureDesc.SampleDesc.Quality = msaaQuality > 0 ? msaaQuality - 1 : 0;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	GFX_THROW_INFO(device->CreateTexture2D(&textureDesc, nullptr, &zBuffer));

	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
	dsvDesc.Format = textureDesc.Format;
	dsvDesc.ViewDimension = msaaQuality > 0 ? D3D11_DSV_DIMENSION_TEXTURE2DMS : D3D11_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Texture2D.MipSlice = 0;
	GFX_THROW_INFO(device->CreateDepthStencilView(zBuffer.Get(), &dsvDesc, &depthStencilView));
}

void Graphics::RenewSize()
{
	RECT rect;
	GetClientRect(hWnd, &rect);
	width = static_cast<uint16_t>(rect.right - rect.left);
	height = static_cast<uint16_t>(rect.bottom - rect.top);
	
	if (width != 0 && height != 0)
	{
		mAspectRatio = 1.0f * width / height;
	}
}

void Graphics::SetViewport()
{
	context->OMSetRenderTargets(1, renderTargetView.GetAddressOf(), depthStencilView.Get());
	D3D11_VIEWPORT vp = { 0 };
	vp.Width = width;
	vp.Height = height;
	vp.MaxDepth = 1;
	context->RSSetViewports(1, &vp);
}

void Graphics::UpdateScene()
{
	namespace wrl = Microsoft::WRL;

	struct Color
	{
		float r;
		float g;
		float b;
		float a;
	};

	struct ConstantBuffer
	{
		dx::XMMATRIX transformation;
	};

	const ConstantBuffer cb =
	{
		{
			dx::XMMatrixTranspose(mRenderData->camera.GetPerspectiveViewTransform(mAspectRatio))
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
}

void Graphics::SetRenderData(RenderData* newData)
{
	mRenderData = newData;
}

void Graphics::DrawMesh(SceneObject<GeometryMesh>& mesh)
{
	if (!mesh.object.IsInitialized())
	{
		mesh.object.InitializeBuffers(device.Get());
	}

	mesh.object.SetBuffers(context.Get());
	GFX_THROW_INFO_VOID(context->DrawIndexed(mesh.object.GetIndexCount(), 0, 0));
}

void Graphics::DrawScene() 
{
	for (auto& mesh : mRenderData->scene.GetMeshes())
	{
		DrawMesh(mesh);
	}
}

void Graphics::DrawUI()
{
	if (ImDrawData* drawData = ImGui::GetDrawData())
	{
		ImGui_ImplDX11_RenderDrawData(drawData);
	}
}

void Graphics::HotReload()
{
	mCurrentTechnique->Rebuild(device);
	mCurrentTechnique->Bind(context);
}

const std::string& Graphics::GetCurrentTechniqueName() const
{
	static const std::string none("none");

	if (!mCurrentTechnique)
	{
		return none;
	}

	return mCurrentTechnique->GetName();
}

const std::vector<std::string>& Graphics::GetAvailableTechniques() const
{
	return TechniqueFactory::GetAvailableTechniques();
}

void Graphics::SetTechnique(const std::string& name)
{
	if (mCurrentTechnique->GetName() == name)
	{
		return;
	}

	mCurrentTechnique.emplace(TechniqueFactory::CreateTechnique(name));
	mCurrentTechnique->Load(device);
	mCurrentTechnique->Bind(context);
}
