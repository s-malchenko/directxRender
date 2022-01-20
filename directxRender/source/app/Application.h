#pragma once

#include "app/Window.h"
#include "renderSystem/RenderData.h"
#include "utility/Timer.h"
#include "utility/DataSwapper.h"

#include <atomic>
#include <thread>

class Application
{
public:
	Application();
	~Application();
	int Run();

private:
	void ProceedFrame();
	void HandleInputs();
	void HandleUI();
	void HandleWindowInactive();
	Window window;
	Timer worldTimer;
	Timer appTimer;
	RenderData mRenderData;

	using RenderSwapper = DataSwapper<RenderData>;
	RenderSwapper mRenderSwapper;
	std::atomic_bool mRunning = false;
	std::thread mRenderThread;
};
