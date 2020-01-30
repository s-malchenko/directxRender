#pragma once

#include "EasyWin.h"
#include "HrException.h"
#include "DxgiInfoManager.h"
#include <d3d11.h>
#include <vector>
#include <string>

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
	~Graphics();
	void EndFrame();
	void ClearBuffer(float red, float green, float blue) noexcept;

private:
	ID3D11Device* device = nullptr;
	IDXGISwapChain* swapChain = nullptr;
	ID3D11DeviceContext* context = nullptr;
	ID3D11RenderTargetView* targetView = nullptr;
#ifndef NDEBUG
	DxgiInfoManager infoManager;
#endif
};

#define GFX_EXCEPT_NOINFO(hr) Graphics::Exception(__LINE__, __FILE__, (hr))
#define GFX_THROW_NOINFO(hrcall) { if (HRESULT _hResult; FAILED(_hResult = (hrcall))) throw Graphics::Exception(__FILE__, __LINE__, _hResult); }

#ifndef NDEEBUG
#define GFX_EXCEPT(hr) Graphics::Exception(__FILE__, __LINE__, (hr), infoManager.GetMessages())
#define GFX_THROW_INFO(hrcall) { infoManager.Set(); if (HRESULT _hResult; FAILED(_hResult = (hrcall))) throw Graphics::Exception(__FILE__, __LINE__, _hResult, infoManager.GetMessages()); }
#define GFX_DEVICE_REMOVED_EXCEPT(_hResult) Graphics::DeviceRemovedException(__FILE__, __LINE__, _hResult, infoManager.GetMessages())
#else
#define GFX_EXCEPT(hr) GFX_EXCEPT_NOINFO(hr)
#define GFX_THROW_INFO(hrcall) GFX_THROW_NOINFO(hrcall)
#define GFX_DEVICE_REMOVED_EXCEPT(_hResult) Graphics::DeviceRemovedException(__FILE__, __LINE__, _hResult)
#endif
