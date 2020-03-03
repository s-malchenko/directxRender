#pragma once

#include "Window.h"
#include "Timer.h"

class Application
{
public:
	Application();
	int Run();
private:
	void ProceedFrame();
	void HandleInputs();
	void HandleWindowInactive();
	Window window;
	Timer worldTimer;
	Timer appTimer;
};

