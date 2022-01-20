#include "Window.h"

#include "../../resource.h"
#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/imgui.h>
#include <sstream>

#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

static void GlfwErrorCallback(int error, const char* description)
{
	std::stringstream ss;
	ss << "GLFW Error: " << description << "\nError code: " << error;
	throw std::runtime_error(ss.str());
}

static void GlfwScrollCallback(GLFWwindow* window, double xOffset, double yOffset)
{
	auto myWindow = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
	myWindow->AddScrollInput(yOffset);
}

static void GlfwMouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
	auto myWindow = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
	myWindow->PushInputEvent({ Window::InputEvent::Device::MOUSE, button, action });
}

static void GlfwKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	auto myWindow = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
	myWindow->PushInputEvent({ Window::InputEvent::Device::KEYBOARD, key, action });
}

static void GlfwWindowFocusCallback(GLFWwindow* window, int focused)
{
	auto myWindow = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
	myWindow->active = focused;
}

static void GlfwFramebufferSizeCallback(GLFWwindow* window, int width, int height)
{
	auto myWindow = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
	myWindow->Gfx().QueryResize();
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

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();

	hWnd = glfwGetWin32Window(mGlfwWindow);
	pGfx = std::make_unique<Graphics>(this->hWnd);

	SetIcon();
	glfwSetWindowUserPointer(mGlfwWindow, this);
	glfwSetKeyCallback(mGlfwWindow, GlfwKeyCallback);
	glfwSetMouseButtonCallback(mGlfwWindow, GlfwMouseButtonCallback);
	glfwSetScrollCallback(mGlfwWindow, GlfwScrollCallback);
	glfwSetWindowFocusCallback(mGlfwWindow, GlfwWindowFocusCallback);
	glfwSetFramebufferSizeCallback(mGlfwWindow, GlfwFramebufferSizeCallback);

	ImGui_ImplGlfw_InitForOther(mGlfwWindow, true); // set ImGui callbacks automatically, callbacks set by user will be called inside ImGui callbacks
}

Window::~Window()
{
	if (pGfx)
	{
		pGfx.reset();
	}

	if (mGlfwWindow)
	{
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
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

bool Window::KeyPressed(int key) const
{
	return glfwGetKey(mGlfwWindow, key) == GLFW_PRESS;
}

bool Window::MouseButtonPressed(int mouseButton) const
{
	return glfwGetMouseButton(mGlfwWindow, mouseButton) == GLFW_PRESS;
}

Window::InputEvent Window::PopInputEvent()
{
	InputEvent event{};

	if (!mInputEvents.empty())
	{
		event = mInputEvents.front();
		mInputEvents.pop();
	}

	return event;
}

Window::CursorPosition Window::GetCursorPosition() const
{
	double x, y;
	glfwGetCursorPos(mGlfwWindow, &x, &y);
	return { static_cast<float>(x), static_cast<float>(y) };
}

Graphics& Window::Gfx()
{
	if (!pGfx)
	{
		throw EXT_EXCEPT("Window Graphics not initialized");
	}

	return *pGfx;
}

void Window::HandleResize(GLFWwindow* window, int width, int height) noexcept
{
	if (pGfx)
	{
		Gfx().HandleWindowResize();
	}
}

void Window::NewFrameUI()
{
	Gfx().NewFrameUI();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
}

void Window::EndFrameUI()
{
	ImGui::Render();
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
	std::vector<uint8_t> pixels(NUM_PIXELS * 4);
	if (!GetDIBits(hdc, info.hbmColor, 0, bmInfo.bmiHeader.biHeight, (LPVOID)pixels.data(), &bmInfo, DIB_RGB_COLORS))
	{
		throw WND_EXCEPT_LASTERR();
	}

	std::vector<uint8_t> actualPixels(NUM_PIXELS * 4);

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
	icon.pixels = actualPixels.data();
	glfwSetWindowIcon(mGlfwWindow, 1, &icon);
}

void Window::PushInputEvent(InputEvent event)
{
	static constexpr int MAX_INPUT_EVENTS = 16;
	mInputEvents.push(event);

	while (mInputEvents.size() > MAX_INPUT_EVENTS)
	{
		mInputEvents.pop();
	}
}

const char* Window::Exception::GetType() const noexcept
{
	return "Window Exception";
}
