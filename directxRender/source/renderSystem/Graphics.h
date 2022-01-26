#pragma once

#include "utility/EasyWin.h"
#include "scene/MeshPrimitives.h"
#include "renderSystem/RenderData.h"

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
	~Graphics();
	Graphics& operator=(const Graphics&) = delete;
	void NewFrameUI();
	void EndFrame();

	void ClearBuffer() noexcept;
	void HandleWindowResize();

	void UpdateScene();
	void SetRenderData(const RenderData* newData);
	void DrawScene();
	void DrawUI();
	void HotReload();
	void QueryResize() { mResizeNeeded = true; }

private:
	Microsoft::WRL::ComPtr<ID3D11Device> device;
	Microsoft::WRL::ComPtr<IDXGISwapChain> swapChain;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> context;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> renderTargetView;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> depthStencilView;
	const RenderData* mRenderData;
	float mAspectRatio = 1;
	HWND hWnd;
	uint16_t width, height;
	const uint16_t sampleCount = 4;
	UINT msaaQuality;
	bool mResizeNeeded = false;

	void CreateDeviceAndContext();
	void CreateSwapChain();
	void CreateRenderTargetView();
	void CreateDepthStencilView();
	void RenewSize();
	void SetViewport();
	void BuildGeometryBuffers(const SceneObject<GeometryMesh>& mesh);
	void DrawMesh(const SceneObject<GeometryMesh>& mesh);
};
