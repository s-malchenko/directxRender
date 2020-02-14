#pragma once

#include "EasyWin.h"
#include "DxgiInfoManager.h"
#include "HrException.h"
#include "MeshPrimitives.h"
#include <d3d11.h>
#include <string>
#include <vector>
#include <wrl.h>

class Graphics
{
public:
	class Exception : public HrException
	{
	public:
		Exception(const char* file, int line, HRESULT hr, const std::vector<std::string>& messages = {});
		const char* what() const override;
		const char* GetType() const noexcept override;
		const std::string& GetErrorInfo() const;
	private:
		std::string info;
	};

	class DeviceRemovedException : public Exception
	{
	public:
		DeviceRemovedException(const char* file, int line, HRESULT hr, const std::vector<std::string>& messages = {});
		const char* GetType() const noexcept override;
	};

	Graphics(HWND hWnd);
	Graphics(const Graphics&) = delete;
	Graphics& operator=(const Graphics&) = delete;
	void EndFrame();
	void ClearBuffer(float red, float green, float blue) noexcept;
	void HandleWindowResize();
	void DrawPrimitiveMesh(const MeshPrimitive& mesh, float angle, float xOffset, float zOffset);

private:
	Microsoft::WRL::ComPtr<ID3D11Device> device;
	Microsoft::WRL::ComPtr<IDXGISwapChain> swapChain;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> context;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> targetView;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> depthStencilView;
	HWND hWnd;
	uint16_t width, height;
	float aspect;
#ifndef NDEBUG
	DxgiInfoManager infoManager;
#endif

};

#define GFX_EXCEPT_NOINFO(hr) Graphics::Exception(__FILE__, __LINE__, (hr))
#define GFX_THROW_NOINFO(hrcall) { if (HRESULT _hResult; FAILED(_hResult = (hrcall))) throw Graphics::Exception(__FILE__, __LINE__, _hResult); }

#ifndef NDEBUG
#define GFX_EXCEPT(hr) Graphics::Exception(__FILE__, __LINE__, (hr), infoManager.GetMessages())
#define GFX_DEVICE_REMOVED_EXCEPT(_hResult) Graphics::DeviceRemovedException(__FILE__, __LINE__, _hResult, infoManager.GetMessages())
#define GFX_THROW_INFO(hrcall) \
{ \
	infoManager.Set(); \
	if (HRESULT _hResult; FAILED(_hResult = (hrcall))) \
	{ \
		throw Graphics::Exception(__FILE__, __LINE__, _hResult, infoManager.GetMessages()); \
	} \
}

#define GFX_THROW_INFO_VOID(voidcall) \
{ \
	infoManager.Set(); \
	(voidcall); \
	auto info = infoManager.GetMessages(); \
	if (!info.empty()) \
	{ \
		throw Graphics::Exception(__FILE__, __LINE__, E_FAIL, info); \
	} \
}

#else
#define GFX_EXCEPT(hr) GFX_EXCEPT_NOINFO(hr)
#define GFX_DEVICE_REMOVED_EXCEPT(_hResult) Graphics::DeviceRemovedException(__FILE__, __LINE__, _hResult)
#define GFX_THROW_INFO(hrcall) GFX_THROW_NOINFO(hrcall)
#define GFX_THROW_INFO_VOID(voidcall) (voidcall)
#endif
