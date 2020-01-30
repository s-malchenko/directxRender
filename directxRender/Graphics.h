#pragma once

#include "EasyWin.h"
#include "HrException.h"
#include <d3d11.h>

class Graphics
{
public:
	class Exception : public HrException
	{
		using HrException::HrException;
	public:
		const char* GetType() const noexcept override;
	};

	class DeviceRemovedException : public Exception
	{
		using Exception::Exception;
	public:
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
};

