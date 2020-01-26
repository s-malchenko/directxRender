#include "Window.h"

#include "resource.h"
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
	wc.hIcon = static_cast<HICON>(LoadImage(hInst, MAKEINTRESOURCE(IDI_ICON1), IMAGE_ICON, 128, 128, 0));
	wc.hIconSm = static_cast<HICON>(LoadImage(hInst, MAKEINTRESOURCE(IDI_ICON1), IMAGE_ICON, 32, 32, 0));
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
	
	if (!AdjustWindowRect(&rect, style, FALSE))
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
	std::stringstream ss;

	switch (msg)
	{
	case WM_CLOSE:
		PostQuitMessage(0);
		break;
	case WM_LBUTTONDOWN:
		const POINTS pt = MAKEPOINTS(lParam);
		ss << "Mouse at (" << pt.x << ":" << pt.y << ")" << std::endl;
		break;
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
		if (!(lParam & (1 << 30)) || keyboard.AutorepeatEnabled())
		{
			keyboard.OnKeyPressed(static_cast<uint8_t>(wParam));
			ss << "Key pressed " << wParam;
		}
		else
		{
			ss << "Autorepeat filtered " << wParam;
		}
		break;
	case WM_KEYUP:
		keyboard.OnKeyReleased(static_cast<uint8_t>(wParam));
		ss << "Key released " << wParam;
		break;
	case WM_CHAR:
		keyboard.OnChar(static_cast<char>(wParam));
		break;
	case WM_KILLFOCUS:
		// release all keys when lose focus
		keyboard.ClearState();
		ss << "Focus lost!";
		break;
	}

	auto str = ss.str();

	if (!str.empty())
	{
		SetWindowText(hWnd, str.c_str());
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