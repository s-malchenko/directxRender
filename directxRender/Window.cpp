#include "Window.h"

#include <sstream>

Window::WindowClass Window::WindowClass::windowClass;

const char* Window::WindowClass::GetName() noexcept
{
	return name;
}

HINSTANCE Window::WindowClass::GetInstance() noexcept
{
	return windowClass.hInst;
}

Window::WindowClass::WindowClass() noexcept
	: hInst(GetModuleHandle(nullptr))
{
	WNDCLASSEX wc = { 0 };
	wc.cbSize = sizeof(wc);
	wc.style = CS_OWNDC;
	wc.lpfnWndProc = HandleMsgSetup;
	wc.hInstance = GetInstance();
	wc.lpszClassName = GetName();
	RegisterClassEx(&wc);
}

Window::WindowClass::~WindowClass()
{
	UnregisterClass(GetName(), GetInstance());
}

Window::Window(const char* name, int width, int height) noexcept
{
	//calculate window size to fit desired region size
	//required values positive enough to subtract some pixels and get positive value
	RECT rect = { 100, 100, 100, 100 };
	rect.right = rect.left + width;
	rect.bottom = rect.top + height;
	const DWORD style = WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU;
	AdjustWindowRect(&rect, style, FALSE);

	hWnd = CreateWindow(
		WindowClass::GetName(),
		name,
		style,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		rect.right - rect.left,
		rect.bottom - rect.top,
		nullptr,
		nullptr,
		WindowClass::GetInstance(),
		this
	);

	ShowWindow(hWnd, SW_SHOWDEFAULT);
}

Window::~Window()
{
	DestroyWindow(hWnd);
}

LRESULT Window::HandleMsgSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	//setup user data and real callback for message processing
	if (msg == WM_NCCREATE)
	{
		//extract Window from creation data
		const auto pCreate = reinterpret_cast<CREATESTRUCTW* const>(lParam);
		const auto pWindow = reinterpret_cast<Window* const>(pCreate->lpCreateParams);
		//store ptr to Window in user data of hWnd
		SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pWindow));
		//set real callback
		SetWindowLongPtr(hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&Window::DeliverMsg));
		//process message
		return pWindow->HandleMsg(hWnd, msg, wParam, lParam);
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}

LRESULT Window::DeliverMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	auto pWindow = reinterpret_cast<Window*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
	return pWindow->HandleMsg(hWnd, msg, wParam, lParam);
}

LRESULT Window::HandleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_CLOSE:
		PostQuitMessage(0);
		break;
	case WM_LBUTTONDOWN:
		const POINTS pt = MAKEPOINTS(lParam);
		std::stringstream ss;
		ss << "Mouse at (" << pt.x << ":" << pt.y << ")" << std::endl;
		SetWindowText(hWnd, ss.str().c_str());
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}
