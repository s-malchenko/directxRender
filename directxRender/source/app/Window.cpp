#include "Window.h"

#include "../../resource.h"
#include <sstream>

#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

static void GlfwErrorCallback(int error, const char* description)
{
	std::stringstream ss;
	ss << "GLFW Error: " << description << "\nError code: " << error;
	throw std::runtime_error(ss.str());
}

Window::Window(const char* name, int width, int height)
{
	glfwSetErrorCallback(GlfwErrorCallback);

	if (!glfwInit())
	{
		throw std::runtime_error("GLFW initialization failed");
	}

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

	if (!(mGlfwWindow = glfwCreateWindow(width, height, name, nullptr, nullptr)))
	{
		throw std::runtime_error("GLFW window creation failed");
	}

	hWnd = glfwGetWin32Window(mGlfwWindow);
	pGfx = std::make_unique<Graphics>(this->hWnd);

	SetIcon();
}

Window::~Window()
{
	if (mGlfwWindow)
	{
		glfwDestroyWindow(mGlfwWindow);
	}

	glfwTerminate();
}

void Window::SetTitle(const std::string& title)
{
	glfwSetWindowTitle(mGlfwWindow, title.c_str());
}

bool Window::Active() const
{
	return active;
}

bool Window::ShouldClose() const
{
	return glfwWindowShouldClose(mGlfwWindow);
}

void Window::ProcessMessages()
{
	glfwPollEvents();
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
/*
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
*/

void Window::HandleResize(GLFWwindow* window, int width, int height) noexcept
{
	if (pGfx)
	{
		Gfx().HandleWindowResize();
	}
}

void Window::SetIcon()
{
	static constexpr int ICON_SIZE = 128;
	static constexpr int NUM_PIXELS = ICON_SIZE * ICON_SIZE;
	auto hIcon = static_cast<HICON>(LoadImage(GetModuleHandle(0), MAKEINTRESOURCE(IDI_ICON1), IMAGE_ICON, ICON_SIZE, ICON_SIZE, 0));
	ICONINFO info = { 0 };
	GetIconInfo(hIcon, &info);
	HDC hdc = GetDC(hWnd);
	BITMAPINFO bmInfo = { 0 };
	bmInfo.bmiHeader.biSize = sizeof(bmInfo.bmiHeader);

	if (!GetDIBits(hdc, info.hbmColor, 0, 0, nullptr, &bmInfo, DIB_RGB_COLORS))
	{
		throw WND_EXCEPT_LASTERR();
	}

	bmInfo.bmiHeader.biCompression = BI_RGB;
	uint8_t pixels[NUM_PIXELS * 4];
	if (!GetDIBits(hdc, info.hbmColor, 0, bmInfo.bmiHeader.biHeight, (LPVOID)pixels, &bmInfo, DIB_RGB_COLORS))
	{
		throw WND_EXCEPT_LASTERR();
	}

	uint8_t actualPixels[NUM_PIXELS * 4];

	for (int rowIndex = 0; rowIndex < ICON_SIZE; ++rowIndex)
	{
		const int originalRowIndex = ICON_SIZE - 1 - rowIndex;

		for (int columnIndex = 0; columnIndex < ICON_SIZE; ++columnIndex)
		{
			const int originalByteIndex = (originalRowIndex * ICON_SIZE + columnIndex) * 4;
			const int glfwByteIndex = (rowIndex * ICON_SIZE + columnIndex) * 4;

			actualPixels[glfwByteIndex] = pixels[originalByteIndex + 2];
			actualPixels[glfwByteIndex + 1] = pixels[originalByteIndex + 1];
			actualPixels[glfwByteIndex + 2] = pixels[originalByteIndex];
			actualPixels[glfwByteIndex + 3] = pixels[originalByteIndex + 3];
		}
	}

	GLFWimage icon;
	icon.width = ICON_SIZE;
	icon.height = ICON_SIZE;
	icon.pixels = actualPixels;
	glfwSetWindowIcon(mGlfwWindow, 1, &icon);
}

const char* Window::Exception::GetType() const noexcept
{
	return "Window Exception";
}
