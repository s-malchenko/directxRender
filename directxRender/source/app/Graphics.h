#pragma once

#include "utility/EasyWin.h"
#include "geometry/MeshPrimitives.h"
#include "scene/PerspectiveCamera.h"
#include "scene/Scene.h"

#include <d3d11.h>
#include <memory>
#include <string>
#include <vector>
#include <wrl.h>

class Graphics
{
public:
	Graphics(HWND hWnd);
	Graphics(const Graphics&) = delete;
	Graphics& operator=(const Graphics&) = delete;
	void EndFrame();

	/**
	 * Clear render target view with given normalized color.
	 */
	void ClearBuffer(float red, float green, float blue) noexcept;
	void HandleWindowResize();

	/**
	 * Update scene. Param dt - time delta in seconds.
	 */
	void UpdateScene(float dt);
	void DrawScene();
	void HotReload();
	PerspectiveCamera& Camera();

private:
	Microsoft::WRL::ComPtr<ID3D11Device> device;
	Microsoft::WRL::ComPtr<IDXGISwapChain> swapChain;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> context;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> renderTargetView;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> depthStencilView;
	Scene scene;
	HWND hWnd;
	std::unique_ptr<PerspectiveCamera> pCam;
	uint16_t width, height;
	const uint16_t sampleCount = 4;
	UINT msaaQuality;
	void CreateDeviceAndContext();
	void CreateSwapChain();
	void CreateRenderTargetView();
	void CreateDepthStencilView();
	void RenewSize();
	void SetViewport();
	void PopulateScene();
	void BuildGeometryBuffers();
};
