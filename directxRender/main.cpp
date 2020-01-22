#include <Windows.h>

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	const auto pClassName = "dx render";
	WNDCLASSEX wc = { 0 };
	wc.cbSize = sizeof(wc);
	wc.style = CS_OWNDC;
	wc.lpfnWndProc = DefWindowProc;
	wc.hInstance = hInstance;
	wc.lpszClassName = pClassName;
	RegisterClassEx(&wc);
	HWND hWnd = CreateWindowEx(
		0,
		pClassName,
		"DX Render",
		WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU,
		200, 200, 640, 480,
		nullptr,
		nullptr,
		hInstance,
		nullptr
	);
	ShowWindow(hWnd, SW_SHOW);
	while (true);
	return 0;
}