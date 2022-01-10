#include "Application.h"

#include "utility/DataSwapper.h"
#include "utility/Util.h"

#include <sstream>
#include <iomanip>

Application::Application() : window("DX render window", 800, 500)
{
	window.SetActivationHandler([this]() { this->HandleWindowInactive(); });
}

Application::~Application()
{
	mRunning = false;

	if (mRenderThread.joinable())
	{
		mRenderThread.join();
	}
}

int Application::Run()
{
	worldTimer.Reset();
	appTimer.Reset();
	mRunning = true;

	mRenderData.scene.Clear();
	mRenderData.scene.LoadFromFile("../../scenes/utah_teapot/small_teapots.obj");

	std::exception_ptr renderThreadException;

	mRenderThread = std::thread([this, &renderThreadException]()
	{
		try
		{
			while (mRunning)
			{
				const RenderData* data;

				while (!(data = mRenderSwapper.TryGetDataForRead()) && mRunning)
				{
					Util::SleepUs(1);
				}

				if (!mRunning)
				{
					return;
				}

				window.Gfx().SetRenderData(data);
				window.Gfx().ClearBuffer();
				window.Gfx().UpdateScene();
				window.Gfx().DrawScene();
				window.Gfx().EndFrame();
				mRenderSwapper.ReaderDone();
			}
		}
		catch (...)
		{
			renderThreadException = std::current_exception();
		}
	});

	while (!window.ShouldClose())
	{
		window.ProcessMessages();

		if (renderThreadException)
		{
			std::rethrow_exception(renderThreadException);
		}

		ProceedFrame();

		RenderData* dataToSwap;
		while (!(dataToSwap = mRenderSwapper.TryGetDataForWrite()))
		{
			Util::SleepUs(1);
		}

		*dataToSwap = mRenderData;
		mRenderSwapper.WriterDone();
	}

	return EXIT_SUCCESS;
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
	HandleInputs();
}

void Application::HandleInputs()
{
	const auto deltaTime = appTimer.Delta();
	auto& mouse = window.GetMouse();
	auto& keyboard = window.GetKeyboard();
	std::ostringstream ss;
	static Mouse::Position prevCursor = {};
	auto cursor = mouse.GetPosition();
	static constexpr float FPS_SHOW_PERIOD_SEC = .5f;
	static float nextFpsShowTime = 0;
	
	if (appTimer.GetPassedTime() > nextFpsShowTime)
	{
		ss << "FPS " << 1 / deltaTime;
		window.SetTitle(ss.str().c_str());
		nextFpsShowTime = appTimer.GetPassedTime() + FPS_SHOW_PERIOD_SEC;
	}

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
				mRenderData.camera.MultiplySpeed(2);
			}
			else
			{
				mRenderData.camera.MultiplySpeed(0.5f);
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
		case Mouse::Event::Type::WheelUp:
			mRenderData.camera.MultiplySpeed(speedMultiplier);
			break;
		case Mouse::Event::Type::WheelDown:
			mRenderData.camera.MultiplySpeed(1 / speedMultiplier);
			break;
		}
	}

	mRenderData.camera.Move(dPos[0], dPos[1], dPos[2]);

	if (mouse.RightPressed())
	{
		mRenderData.camera.Turn((prevCursor.y - cursor.y) / 200.0f, (cursor.x - prevCursor.x) / 200.0f);
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
