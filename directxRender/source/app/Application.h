#pragma once

#include "app/Window.h"
#include "utility/Timer.h"

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
