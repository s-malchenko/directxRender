#include "Application.h"

#include "Timer.h"
#include "Util.h"
#include <sstream>
#include <iomanip>

Application::Application() : window("DX render window")
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
	ss << "Time passed " << std::setprecision(1) << std::fixed << time << "s";
	window.SetTitle(ss.str().c_str());
	float red = (std::sin(time) + 1) / 2;
	float green = (std::cos(time * 1.3f) + 1) / 2;
	float blue = (std::cos(time * 0.7f) + 1) / 2;
	window.Gfx().ClearBuffer(red, green, blue);
	window.Gfx().DrawTestTriangle();
	window.Gfx().EndFrame();
}
