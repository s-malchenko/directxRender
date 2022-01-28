#pragma once

#include "utility/EasyWin.h"
#include "pipeline/Technique.h"
#include "renderSystem/RenderData.h"

#include "renderSystem/EasyD3D11.h"
#include <memory>
#include <optional>
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
	void SetRenderData(RenderData* newData);
	void DrawScene();
	void DrawUI();
	void HotReload();
	void QueryResize() { mResizeNeeded = true; }

	const std::string& GetCurrentTechniqueName() const;
	const std::vector<std::string>& GetAvailableTechniques() const;
	void SetTechnique(const std::string& name);
	

private:
	Microsoft::WRL::ComPtr<ID3D11Device> device;
	Microsoft::WRL::ComPtr<IDXGISwapChain> swapChain;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> context;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> renderTargetView;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> depthStencilView;
	RenderData* mRenderData;
	float mAspectRatio = 1;
	HWND hWnd;
	uint16_t width, height;
	const uint16_t sampleCount = 4;
	UINT msaaQuality;
	bool mResizeNeeded = false;
	std::optional<Technique> mCurrentTechnique;

	void CreateDeviceAndContext();
	void CreateSwapChain();
	void CreateRenderTargetView();
	void CreateDepthStencilView();
	void RenewSize();
	void SetViewport();
	void DrawMesh(SceneObject<Scene::MeshWithMaterial>& mesh);
};
