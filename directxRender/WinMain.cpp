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

		while (result = GetMessage(&msg, nullptr, 0, 0) > 0)
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);

			while (!window.mouse.Empty())
			{
				const auto event = window.mouse.Read();

				if (event.GetType() == Mouse::Event::Type::Move)
				{
					std::ostringstream ss;
					ss << "Mouse moved to " << event.GetPosition().x << ":" << event.GetPosition().y;
					window.SetTitle(ss.str());
				}
				else if (event.GetType() == Mouse::Event::Type::WheelUp)
				{
					window.SetTitle("Mouse wheel up");
				}
				else if (event.GetType() == Mouse::Event::Type::WheelDown)
				{
					window.SetTitle("Mouse wheel down");
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
