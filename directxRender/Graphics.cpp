#include "Graphics.h"

#pragma comment(lib, "d3d11.lib")

#define GFX_THROW_FAILED(hrcall) { if (HRESULT _hResult; FAILED(_hResult = (hrcall))) throw Graphics::Exception(__FILE__, __LINE__, _hResult); }
#define GFX_DEVICE_REMOVED_EXCEPT(_hResult) Graphics::DeviceRemovedException(__FILE__, __LINE__, _hResult)

Graphics::Graphics(HWND hWnd)
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

	GFX_THROW_FAILED(
		D3D11CreateDeviceAndSwapChain(
			nullptr,
			D3D_DRIVER_TYPE_HARDWARE,
			nullptr,
			D3D11_CREATE_DEVICE_DEBUG,
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
	ID3D11Resource* backBuffer = nullptr;
	GFX_THROW_FAILED(swapChain->GetBuffer(0, __uuidof(ID3D11Resource), reinterpret_cast<void**>(&backBuffer)));
	GFX_THROW_FAILED(device->CreateRenderTargetView(backBuffer, nullptr, &targetView));
	backBuffer->Release();
}

#define RELEASE(ptr) { if (ptr) ptr->Release(); }

Graphics::~Graphics()
{
	RELEASE(device);
	RELEASE(swapChain);
	RELEASE(context);
}

void Graphics::EndFrame()
{
	if (const auto hr = swapChain->Present(1, 0); FAILED(hr))
	{
		if (hr == DXGI_ERROR_DEVICE_REMOVED)
		{
			throw GFX_DEVICE_REMOVED_EXCEPT(device->GetDeviceRemovedReason());
		}
		
		GFX_THROW_FAILED(hr);
	}
}

void Graphics::ClearBuffer(float red, float green, float blue) noexcept
{
	const float color[] = { red, green, blue, 1 };
	context->ClearRenderTargetView(targetView, color);
}

const char * Graphics::Exception::GetType() const noexcept
{
	return "Graphics Exception";
}

const char * Graphics::DeviceRemovedException::GetType() const noexcept
{
	return "DeviceRemoved Exception";
}
