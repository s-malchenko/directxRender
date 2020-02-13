#include "Application.h"

#include "Timer.h"
#include "Util.h"
#include <sstream>
#include <iomanip>

Application::Application() : window("DX render window", 800, 500)
{
}

int Application::Run()
{
	while (true)
	{
		if (const auto code = window.ProcessMessages())
		{
			return *code;
		}

		ProceedFrame();
		Util::SleepMs(10);
	}
}

void Application::ProceedFrame()
{
	static Timer timer;
	std::ostringstream ss;
	const auto time = timer.Peek();
	float red = (std::sin(time) + 1) / 2;
	float green = (std::cos(time * 1.3f) + 1) / 2;
	float blue = (std::cos(time * 0.7f) + 1) / 2;
	window.Gfx().ClearBuffer(red, green, blue);
	auto cursor = window.mouse.GetPosition();
	ss << "Cursor at " << cursor.x << ":" << cursor.y;
	window.SetTitle(ss.str().c_str());
	window.Gfx().DrawTestCube(time, 0, 0);
	window.Gfx().DrawTestCube(-time, 0, 1 - cursor.y * 2.0f / window.GetHeight());
	window.Gfx().EndFrame();
}
