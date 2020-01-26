#include "EasyWin.h"
#include "Window.h"

#include <sstream>

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	try
	{
		Window window("DX render window");

		//processing messages
		MSG msg;
		BOOL result;

		int wheelDelta = 0;
		while (result = GetMessage(&msg, nullptr, 0, 0) > 0)
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);

			while (!window.mouse.Empty())
			{
				std::ostringstream ss;
				const auto event = window.mouse.Read();

				switch (event.GetType())
				{
				case Mouse::Event::Type::Move:
					ss << "Mouse moved to " << event.GetPosition().x << ":" << event.GetPosition().y;
					break;
				case Mouse::Event::Type::WheelUp:
					++wheelDelta;
					ss << "Wheel " << wheelDelta;
					break;
				case Mouse::Event::Type::WheelDown:
					--wheelDelta;
					ss << "Wheel " << wheelDelta;
					break;
				case Mouse::Event::EnterRegion:
					ss << "Enter";
					break;
				case Mouse::Event::LeaveRegion:
					ss << "Leave";
					break;
				}

				if (!ss.str().empty())
				{
					window.SetTitle(ss.str());
				}
			}
		}

		if (result == -1)
		{
			return -1;
		}

		return static_cast<int>(msg.wParam);
	}
	catch (const ExtendedException& e)
	{
		MessageBox(nullptr, e.what(), e.GetType(), MB_OK | MB_ICONEXCLAMATION);
	}
	catch (const std::exception& e)
	{
		MessageBox(nullptr, e.what(), "Standard exception", MB_OK | MB_ICONEXCLAMATION);
	}
	catch (...)
	{
		MessageBox(nullptr, "No information available", "Unknown exception", MB_OK | MB_ICONEXCLAMATION);
	}

	return -1;
}
