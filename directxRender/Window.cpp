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

Window::Window(const char* name, int width, int height)
{
	//calculate window size to fit desired region size
	//required values positive enough to subtract some pixels and get positive value
	RECT rect = { 100, 100, 100, 100 };
	rect.right = rect.left + width;
	rect.bottom = rect.top + height;
	const DWORD style = WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU;
	
	if (FAILED(AdjustWindowRect(&rect, style, FALSE)))
	{
		throw WND_EXCEPT_LASTERR();
	}

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

	if (!hWnd)
	{
		throw WND_EXCEPT_LASTERR();
	}

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

Window::Exception::Exception(const char * file, int line, HRESULT hr)
	: ExtendedException(file, line), hr(hr)
{
}

const char * Window::Exception::what() const
{
	std::ostringstream ss;
	ss << GetType() << std::endl
		<< "[Error code]: " << GetErrorCode() << std::endl
		<< "[Description]: " << GetErrorString() << std::endl
		<< GetOriginString();
	buffer = ss.str();
	return buffer.c_str();
}

const char * Window::Exception::GetType() const noexcept
{
	return "Window Exception";
}

std::string Window::Exception::TranslateErrorCode(HRESULT hr)
{
	char* buf = nullptr;
	DWORD formatFlags = FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS;
	DWORD msgLen = FormatMessage(
		formatFlags,
		nullptr,
		hr,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		reinterpret_cast<LPSTR>(&buf),
		0,
		nullptr
	);

	if (msgLen == 0)
	{
		return "Unknown error code";
	}

	std::string errorString(buf);
	LocalFree(buf);
	return errorString;
}

HRESULT Window::Exception::GetErrorCode() const noexcept
{
	return hr;
}

std::string Window::Exception::GetErrorString() const
{
	return TranslateErrorCode(hr);
}
