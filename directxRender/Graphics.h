#pragma once

#include "EasyWin.h"
#include "DxgiInfoManager.h"
#include "HrException.h"
#include "MeshPrimitives.h"
#include "PerspectiveCamera.h"
#include <d3d11.h>
#include <memory>
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
	PerspectiveCamera& Camera();

private:
	Microsoft::WRL::ComPtr<ID3D11Device> device;
	Microsoft::WRL::ComPtr<IDXGISwapChain> swapChain;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> context;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> targetView;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> depthStencilView;
	HWND hWnd;
	std::unique_ptr<PerspectiveCamera> pCam;
	uint16_t width, height;
	void CreateDeviceAndContext();
	void CreateSwapChain();
#ifndef NDEBUG
	DxgiInfoManager infoManager;
#endif

};
