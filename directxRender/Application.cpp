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
	
	const auto time = timer.Peek();
	const auto deltaTime = frameTimer.Mark();

	float red = (std::sin(time) + 1) / 2;
	float green = (std::cos(time * 1.3f) + 1) / 2;
	float blue = (std::cos(time * 0.7f) + 1) / 2;
	window.Gfx().ClearBuffer(red, green, blue);
	HandleInputs(deltaTime);
	
	window.Gfx().DrawPrimitiveMesh(MeshPrimitives::Cone, 0, 0, 0);
	window.Gfx().DrawPrimitiveMesh(MeshPrimitives::OriginPlane, 0, 0, -0.5);
	window.Gfx().DrawPrimitiveMesh(MeshPrimitives::Cube, -time, 0, 0.5f * std::sin(time * 2));
	window.Gfx().EndFrame();
}

void Application::HandleInputs(float deltaTime)
{
	std::ostringstream ss;
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

	while (!window.keyboard.KeyEmpty())
	{
		auto event = window.keyboard.ReadKey();

		if (event && event.Code() == VK_SHIFT)
		{
			if (event.IsPress())
			{
				window.Gfx().Camera().MultiplySpeed(2);
			}
			else
			{
				window.Gfx().Camera().MultiplySpeed(0.5f);
			}
		}
	}

	static constexpr float speedMultiplier = 1.2f;

	while (!window.mouse.Empty())
	{
		auto event = window.mouse.Read();

		switch (event.GetType())
		{
		case Mouse::Event::WheelUp:
			window.Gfx().Camera().MultiplySpeed(speedMultiplier);
			break;
		case Mouse::Event::WheelDown:
			window.Gfx().Camera().MultiplySpeed(1 / speedMultiplier);
			break;
		}
	}

	window.Gfx().Camera().Move(dPos[0], dPos[1], dPos[2]);

	if (window.mouse.RightPressed())
	{
		window.Gfx().Camera().Turn((prevCursor.y - cursor.y) / 200.0f, (cursor.x - prevCursor.x) / 200.0f);
	}

	prevCursor = cursor;
}
