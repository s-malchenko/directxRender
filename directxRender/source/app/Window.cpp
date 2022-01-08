#include "Window.h"

#include "../../resource.h"
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

Window::Window(const char* name, int width, int height) : width(width), height(height)
{
	//calculate window size to fit desired region size
	//required values positive enough to subtract some pixels and get positive value
	RECT rect = { 100, 100, 100, 100 };
	rect.right = rect.left + width;
	rect.bottom = rect.top + height;
	const DWORD style = WS_OVERLAPPEDWINDOW;
	
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
	pGfx = std::make_unique<Graphics>(this->hWnd);
}

Window::~Window()
{
	DestroyWindow(hWnd);
}

void Window::SetTitle(const std::string& title)
{
	if (!SetWindowText(hWnd, title.c_str()))
	{
		throw WND_EXCEPT_LASTERR();
	}
}

int Window::GetWidth() const noexcept
{
	return width;
}

int Window::GetHeight() const noexcept
{
	return height;
}

bool Window::Active() const
{
	return active;
}

std::optional<int> Window::ProcessMessages()
{
	MSG msg;

	while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
	{
		if (msg.message == WM_QUIT)
		{
			return (int)msg.wParam;
		}

		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return {};
}

void Window::SetActivationHandler(handler_t handler)
{
	onActiveChanged = handler;
}

Mouse& Window::GetMouse()
{
	return mouse;
}

Keyboard& Window::GetKeyboard()
{
	return keyboard;
}

Graphics& Window::Gfx()
{
	if (!pGfx)
	{
		throw EXT_EXCEPT("Window Graphics not initialized");
	}

	return *pGfx;
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

#define MOUSE_POSITION(param) *reinterpret_cast<Mouse::Position *>(&MAKEPOINTS(param))

LRESULT Window::HandleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_CLOSE:
		PostQuitMessage(0);
		break;
	case WM_ACTIVATE:
		if (LOWORD(wParam) == WA_INACTIVE)
		{
			active = false;
		}
		else
		{
			active = true;
		}

		if (onActiveChanged)
		{
			onActiveChanged.value()();
		}
		break;
	case WM_ENTERSIZEMOVE:
		active = false;
		if (onActiveChanged)
		{
			onActiveChanged.value()();
		}
		break;
	case WM_EXITSIZEMOVE:
		active = true;
		if (onActiveChanged)
		{
			onActiveChanged.value()();
		}
		break;
	//mouse messages
	case WM_LBUTTONDOWN:
		mouse.PostEvent(Mouse::Event::Type::LPress, MOUSE_POSITION(lParam));
		break;
	case WM_LBUTTONUP:
		mouse.PostEvent(Mouse::Event::Type::LRelease, MOUSE_POSITION(lParam));

		//button up outside of region, release mouse
		if (!PointsInside(lParam)
			&& !mouse.LeftPressed()
			&& !mouse.RightPressed())
		{
			ReleaseCapture();
			mouse.PostEvent(Mouse::Event::Type::LeaveRegion, MOUSE_POSITION(lParam));
		}
		break;
	case WM_RBUTTONDOWN:
		mouse.PostEvent(Mouse::Event::Type::RPress, MOUSE_POSITION(lParam));
		break;
	case WM_RBUTTONUP:
		mouse.PostEvent(Mouse::Event::Type::RRelease, MOUSE_POSITION(lParam));

		//button up outside of region, release mouse
		if (!PointsInside(lParam)
			&& !mouse.LeftPressed()
			&& !mouse.RightPressed())
		{
			ReleaseCapture();
			mouse.PostEvent(Mouse::Event::Type::LeaveRegion, MOUSE_POSITION(lParam));
		}
		break;
	case WM_MOUSEWHEEL:
		mouse.OnWheelDelta(GET_WHEEL_DELTA_WPARAM(wParam), MOUSE_POSITION(lParam));
		break;
	case WM_MOUSEMOVE:
		if (!PointsInside(lParam)) //mouse moved outside of client region
		{
			if (mouse.LeftPressed() || mouse.RightPressed())
			{
				mouse.PostEvent(Mouse::Event::Type::Move, MOUSE_POSITION(lParam));
			}
			else
			{
				ReleaseCapture();
				mouse.PostEvent(Mouse::Event::Type::LeaveRegion, MOUSE_POSITION(lParam));
			}
		}
		else //mouse is in client region
		{
			mouse.PostEvent(Mouse::Event::Type::Move, MOUSE_POSITION(lParam));

			if (!mouse.Inside())
			{
				SetCapture(hWnd);
				mouse.PostEvent(Mouse::Event::Type::EnterRegion, MOUSE_POSITION(lParam));
			}
		}
		break;

	//keyboard messages
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
		if (!(lParam & (1 << 30)) || keyboard.AutorepeatEnabled())
		{
			keyboard.OnKeyPressed(static_cast<uint8_t>(wParam));
		}
		break;
	case WM_KEYUP:
	case WM_SYSKEYUP:
		keyboard.OnKeyReleased(static_cast<uint8_t>(wParam));
		break;
	case WM_CHAR:
		keyboard.OnChar(static_cast<char>(wParam));
		break;
	case WM_KILLFOCUS:
		// release all keys when lose focus
		keyboard.ClearState();
		break;
	case WM_SIZE:
		HandleResize(lParam);
		break;
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}

bool Window::PointsInside(LPARAM lParam) const noexcept
{
	const auto pos = MAKEPOINTS(lParam);
	return !(
		pos.x < 0
		|| pos.x > width
		|| pos.y < 0
		|| pos.y > height
		);
}

void Window::HandleResize(LPARAM lParam) noexcept
{
	const auto newSize = MAKEPOINTS(lParam);
	width = newSize.x;
	height = newSize.y;

	if (pGfx)
	{
		Gfx().HandleWindowResize();
	}
}

const char* Window::Exception::GetType() const noexcept
{
	return "Window Exception";
}
