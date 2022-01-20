#include "Application.h"

#include "utility/DataSwapper.h"
#include "utility/Util.h"

#include <imgui/imgui.h>
#include <iomanip>
#include <sstream>

Application::Application() : window("DX render window", 800, 500)
{
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
				RenderSwapper::ReaderHolder dataHolder;

				while (!(dataHolder = mRenderSwapper.TryGetDataForRead()) && mRunning)
				{
					Util::SleepUs(1);
				}

				if (!mRunning)
				{
					return;
				}

				if (!window.Active())
				{
					continue;
				}

				window.Gfx().HandleWindowResize();
				window.Gfx().SetRenderData(dataHolder.GetData());
				window.Gfx().ClearBuffer();
				window.Gfx().UpdateScene();
				window.Gfx().DrawScene();
				window.Gfx().DrawUI();
				window.Gfx().EndFrame();
			}
		}
		catch (...)
		{
			renderThreadException = std::current_exception();
		}
	});

	while (!window.ShouldClose())
	{
		HandleWindowInactive();
		window.ProcessMessages();

		if (renderThreadException)
		{
			std::rethrow_exception(renderThreadException);
		}

		ProceedFrame();

		{
			RenderSwapper::WriterHolder dataHolder;
			while (!(dataHolder = mRenderSwapper.TryGetDataForWrite()))
			{
				Util::SleepUs(1);
			}

			*dataHolder.GetData() = mRenderData;
		}
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
	HandleUI();
	HandleInputs();
}

void Application::HandleInputs()
{
	const auto deltaTime = appTimer.Delta();
	std::ostringstream ss;
	static constexpr float FPS_SHOW_PERIOD_SEC = .5f;
	static float nextFpsShowTime = 0;
	
	if (appTimer.GetPassedTime() > nextFpsShowTime)
	{
		ss << "FPS " << 1 / deltaTime;
		window.SetTitle(ss.str().c_str());
		nextFpsShowTime = appTimer.GetPassedTime() + FPS_SHOW_PERIOD_SEC;
	}

	float dPos[] = { 0, 0, 0 };

	if (window.KeyPressed(GLFW_KEY_W))
	{
		dPos[0] = deltaTime;
	}
	else if (window.KeyPressed(GLFW_KEY_S))
	{
		dPos[0] = -deltaTime;
	}

	if (window.KeyPressed(GLFW_KEY_D))
	{
		dPos[1] = deltaTime;
	}
	else if (window.KeyPressed(GLFW_KEY_A))
	{
		dPos[1] = -deltaTime;
	}

	if (window.KeyPressed(GLFW_KEY_SPACE))
	{
		dPos[2] = deltaTime;
	}
	else if (window.KeyPressed(GLFW_KEY_C))
	{
		dPos[2] = -deltaTime;
	}
	
	while (auto event = window.PopInputEvent())
	{
		switch (event.mDevice)
		{
		case Window::InputEvent::Device::KEYBOARD:
			switch (event.mKey)
			{
			case GLFW_KEY_LEFT_SHIFT:
				if (event.mAction == GLFW_PRESS)
				{
					mRenderData.camera.MultiplySpeed(2);
				}
				else if (event.mAction == GLFW_RELEASE)
				{
					mRenderData.camera.MultiplySpeed(0.5f);
				}
				break;
			case GLFW_KEY_F:
				if (event.mAction == GLFW_PRESS)
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
			case GLFW_KEY_Q:
				if (event.mAction == GLFW_PRESS)
				{
					window.Gfx().HotReload();
				}
				break;
			default:
				break;
			}
			break;
		default:
			break;
		}
	}

	static constexpr float speedMultiplier = 1.2f;
	const float scrollInput = window.GetScrollInput();

	if (scrollInput != 0)
	{
		mRenderData.camera.MultiplySpeed(std::pow(speedMultiplier, scrollInput));
		window.ResetScrollInput();
	}

	mRenderData.camera.Move(dPos[0], dPos[1], dPos[2]);

	static Window::CursorPosition prevCursor = {};
	auto cursor = window.GetCursorPosition();

	if (window.MouseButtonPressed(GLFW_MOUSE_BUTTON_RIGHT))
	{
		mRenderData.camera.Turn((prevCursor.y - cursor.y) / 200.0f, (cursor.x - prevCursor.x) / 200.0f);
	}

	prevCursor = cursor;
}

void Application::HandleUI()
{
	auto lock = window.Gfx().AcquireUILock();
	window.NewFrameUI();

	ImGui::Begin("ImGui Window");
	ImGui::Text("Press the button to reset camera position and speed");

	if (ImGui::Button("Reset camera"))
	{
		mRenderData.camera.Reset();
	}

	ImGui::End();
	window.EndFrameUI();
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
