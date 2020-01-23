#include "EasyWin.h"
#include "Window.h"

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	Window window("DX render window");

	//processing messages
	MSG msg;
	BOOL result;

	while (result = GetMessage(&msg, nullptr, 0, 0) > 0)
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	if (result == -1)
	{
		return -1;
	}

	return static_cast<int>(msg.wParam);
}
