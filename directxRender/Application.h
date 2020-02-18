#pragma once

#include "Window.h"

class Application
{
public:
	Application();
	int Run();
private:
	void ProceedFrame();
	void HandleInputs(float deltaTime);
	Window window;
};

