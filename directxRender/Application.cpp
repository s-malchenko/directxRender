#include "Application.h"

#include "Util.h"
#include <sstream>
#include <iomanip>

Application::Application() : window("DX render window", 800, 500)
{
}

int Application::Run()
{
	appTimer.Reset();
	statTimer.Reset();

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
	appTimer.Tick();
	statTimer.Tick();
	const auto appTime = appTimer.GetPassedTime();

	float red = static_cast<float>(std::sin(appTime) + 1) / 2;
	float green = static_cast<float>(std::cos(appTime * 1.3f) + 1) / 2;
	float blue = static_cast<float>(std::cos(appTime * 0.7f) + 1) / 2;
	window.Gfx().ClearBuffer(red, green, blue);
	HandleInputs();
	
	window.Gfx().DrawPrimitiveMesh(MeshPrimitives::Cone, 0, 0, 0);
	window.Gfx().DrawPrimitiveMesh(MeshPrimitives::OriginPlane, 0, 0, -0.5);
	window.Gfx().DrawPrimitiveMesh(MeshPrimitives::Cube, -appTime, 0, 0.5f * std::sin(appTime * 2));
	window.Gfx().EndFrame();
}

void Application::HandleInputs()
{
	const auto deltaTime = statTimer.Delta();
	std::ostringstream ss;
	static Mouse::Position prevCursor = {};
	auto cursor = window.mouse.GetPosition();
	ss << "FPS " << 1 / deltaTime;
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

		if (!event)
		{
			continue;
		}

		switch (event.Code())
		{
		case VK_SHIFT:
			if (event.IsPress())
			{
				window.Gfx().Camera().MultiplySpeed(2);
			}
			else
			{
				window.Gfx().Camera().MultiplySpeed(0.5f);
			}
			break;
		case 'F':
			if(event.IsPress())
			{
				if (appTimer.Paused())
				{
					appTimer.Continue();
				}
				else
				{
					appTimer.Pause();
				}
			}
			break;
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
