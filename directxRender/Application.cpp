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
	ss << "Time passed " << std::setprecision(1) << std::fixed << timer.Peek() << "s";
	window.SetTitle(ss.str().c_str());
}
