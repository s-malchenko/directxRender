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
	static Timer frameTimer;
	std::ostringstream ss;
	const auto time = timer.Peek();
	const auto deltaTime = frameTimer.Mark();

	float red = (std::sin(time) + 1) / 2;
	float green = (std::cos(time * 1.3f) + 1) / 2;
	float blue = (std::cos(time * 0.7f) + 1) / 2;
	window.Gfx().ClearBuffer(red, green, blue);
	static Mouse::Position prevCursor = {};
	auto cursor = window.mouse.GetPosition();
	ss << "Cursor at " << cursor.x << ":" << cursor.y;
	window.SetTitle(ss.str().c_str());

	float dPos[] = { 0, 0, 0 };

	if (window.keyboard.KeyPressed('W'))
	{
		dPos[0] = deltaTime;
	}
	else if (window.keyboard.KeyPressed('S'))
	{
		dPos[0] = -deltaTime;
	}

	if (window.keyboard.KeyPressed('D'))
	{
		dPos[1] = deltaTime;
	}
	else if (window.keyboard.KeyPressed('A'))
	{
		dPos[1] = -deltaTime;
	}

	if (window.keyboard.KeyPressed(' '))
	{
		dPos[2] = deltaTime;
	}
	else if (window.keyboard.KeyPressed('C'))
	{
		dPos[2] = -deltaTime;
	}

	window.Gfx().Camera().Move(dPos[0], dPos[1], dPos[2]);
	
	if (window.mouse.RightPressed())
	{
		window.Gfx().Camera().Turn((prevCursor.y - cursor.y) / 200.0f, (cursor.x - prevCursor.x) / 200.0f);
	}

	prevCursor = cursor;
	window.Gfx().DrawPrimitiveMesh(MeshPrimitives::Cone, 0, 0, 0);
	window.Gfx().DrawPrimitiveMesh(MeshPrimitives::OriginPlane, 0, 0, -0.5);
	window.Gfx().DrawPrimitiveMesh(MeshPrimitives::Cube, -time, 0, 0.5f * std::sin(time * 2));
	window.Gfx().EndFrame();
}
