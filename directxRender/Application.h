#pragma once

#include "Window.h"

class Application
{
public:
	Application();
	int Run();
private:
	void ProceedFrame();
	Window window;
};

