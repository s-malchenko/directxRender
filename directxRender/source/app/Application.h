#pragma once

#include "app/Window.h"
#include "renderSystem/RenderData.h"
#include "utility/Timer.h"

class Application
{
public:
	Application();
	~Application();
	int Run();

private:
	void ProceedFrame();
	void RenderFrame();
	void HandleInputs();
	void HandleUI();
	void HandleWindowInactive();
	Window window;
	Timer worldTimer;
	Timer appTimer;
	RenderData mRenderData;

	bool mRunning = false;
};
