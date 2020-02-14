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
	window.Gfx().DrawPrimitiveMesh(MeshPrimitives::Cube, 0, 0, 0);
	window.Gfx().DrawPrimitiveMesh(MeshPrimitives::Cone, -time, 0, 0.5f * std::sin(time * 2));
	window.Gfx().EndFrame();
}
