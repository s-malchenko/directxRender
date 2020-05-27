#include "Application.h"

#include "utility/Util.h"
#include <sstream>
#include <iomanip>

Application::Application() : window("DX render window", 800, 500)
{
	window.SetActivationHandler([this]() { this->HandleWindowInactive(); });
}

int Application::Run()
{
	worldTimer.Reset();
	appTimer.Reset();

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
	if (!window.Active())
	{
		Util::SleepMs(1);
		return;
	}

	worldTimer.Tick();
	appTimer.Tick();
	const auto appTime = worldTimer.GetPassedTime();

	float red = static_cast<float>(std::sin(appTime) + 1) / 2;
	float green = static_cast<float>(std::cos(appTime * 1.3f) + 1) / 2;
	float blue = static_cast<float>(std::cos(appTime * 0.7f) + 1) / 2;
	window.Gfx().ClearBuffer(red, green, blue);
	HandleInputs();
	
	window.Gfx().UpdateScene(appTimer.Delta());
	window.Gfx().DrawScene();
	window.Gfx().EndFrame();
}

void Application::HandleInputs()
{
	const auto deltaTime = appTimer.Delta();
	auto& mouse = window.GetMouse();
	auto& keyboard = window.GetKeyboard();
	std::ostringstream ss;
	static Mouse::Position prevCursor = {};
	auto cursor = mouse.GetPosition();
	ss << "FPS " << 1 / deltaTime;
	window.SetTitle(ss.str().c_str());

	float dPos[] = { 0, 0, 0 };

	if (keyboard.KeyPressed('W'))
	{
		dPos[0] = deltaTime;
	}
	else if (keyboard.KeyPressed('S'))
	{
		dPos[0] = -deltaTime;
	}

	if (keyboard.KeyPressed('D'))
	{
		dPos[1] = deltaTime;
	}
	else if (keyboard.KeyPressed('A'))
	{
		dPos[1] = -deltaTime;
	}

	if (keyboard.KeyPressed(' '))
	{
		dPos[2] = deltaTime;
	}
	else if (keyboard.KeyPressed('C'))
	{
		dPos[2] = -deltaTime;
	}

	while (!keyboard.KeyEmpty())
	{
		auto event = keyboard.ReadKey();

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
				if (worldTimer.Paused())
				{
					worldTimer.Continue();
				}
				else
				{
					worldTimer.Pause();
				}
			}
			break;
		case 'Q':
			if (event.IsPress())
			{
				window.Gfx().HotReload();
			}
			break;
		}
	}

	static constexpr float speedMultiplier = 1.2f;

	while (!mouse.Empty())
	{
		auto event = mouse.Read();

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

	if (mouse.RightPressed())
	{
		window.Gfx().Camera().Turn((prevCursor.y - cursor.y) / 200.0f, (cursor.x - prevCursor.x) / 200.0f);
	}

	prevCursor = cursor;
}

void Application::HandleWindowInactive()
{
	static bool prevActive = false;
	static bool worldPaused = false;
	static bool appPaused = false;
	bool curActive = window.Active();

	if (prevActive == curActive)
	{
		return;
	}

	if (prevActive)
	{
		worldPaused = worldTimer.Paused();
		appPaused = appTimer.Paused();
		worldTimer.Pause();
		appTimer.Pause();
	}
	else
	{
		if (!worldPaused)
		{
			worldTimer.Continue();
		}

		if (!appPaused)
		{
			appTimer.Continue();
		}
	}

	prevActive = curActive;
}
