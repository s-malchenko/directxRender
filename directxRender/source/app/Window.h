#pragma once

#include "utility/EasyWin.h"
#include "exception/HrException.h"
#include "controls/Keyboard.h"
#include "controls/Mouse.h"
#include "renderSystem/Graphics.h"

#include <functional>
#include <glfw/glfw3.h>
#include <optional>
#include <memory>
#include <string>

class Window
{
	using handler_t = std::function<void()>;
public:
	class Exception : public HrException
	{
		using HrException::HrException;
	public:
		const char* GetType() const noexcept override;
	};

public:
	Window(const char* name, int width = 640, int height = 480);
	~Window();
	Window(const Window&) = delete;
	Window& operator=(const Window&) = delete;
	void SetTitle(const std::string& title);
	int GetWidth() const noexcept;
	bool Active() const;
	void ProcessMessages();
	void SetActivationHandler(handler_t handler);
	bool ShouldClose() const;

	Mouse& GetMouse();
	Keyboard& GetKeyboard();
	Graphics& Gfx();

private:
	void HandleResize(GLFWwindow* window, int width, int height) noexcept;
	void SetIcon();
	
	Mouse mouse;
	Keyboard keyboard;
	HWND hWnd;
	std::unique_ptr<Graphics> pGfx;
	GLFWwindow* mGlfwWindow;
	bool active = false;
	std::optional<handler_t> onActiveChanged;
};

//macro for capturing file and line to exception
#define WND_EXCEPT(hr) Window::Exception(__FILE__, __LINE__, hr)
#define WND_EXCEPT_LASTERR() WND_EXCEPT(GetLastError())
