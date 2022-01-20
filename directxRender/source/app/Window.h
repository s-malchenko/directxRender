#pragma once

#include "exception/HrException.h"
#include "renderSystem/Graphics.h"

#include <atomic>
#include <functional>
#include <glfw/glfw3.h>
#include <optional>
#include <memory>
#include <string>
#include <queue>

class Window
{
	friend void GlfwScrollCallback(GLFWwindow* window, double xOffset, double yOffset);
	friend void GlfwMouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
	friend void GlfwKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
	friend void GlfwWindowFocusCallback(GLFWwindow* window, int focused);
public:
	class Exception : public HrException
	{
		using HrException::HrException;
	public:
		const char* GetType() const noexcept override;
	};

	struct InputEvent
	{
		enum class Device
		{
			NONE,
			KEYBOARD,
			MOUSE,
		};

		operator bool() const
		{
			return mDevice != Device::NONE;
		}

		Device mDevice = Device::NONE;
		int mKey;
		int mAction;
	};

	struct CursorPosition
	{
		float x = 0;
		float y = 0;
	};

public:
	Window(const char* name, int width = 640, int height = 480);
	~Window();
	Window(const Window&) = delete;
	Window& operator=(const Window&) = delete;
	void SetTitle(const std::string& title);
	bool Active() const;
	void ProcessMessages();
	bool ShouldClose() const;
	void NewFrameUI();
	void EndFrameUI();

	bool KeyPressed(int key) const;
	bool MouseButtonPressed(int mouseButton) const;
	float GetScrollInput() const { return mMouseScrollInput; }
	void ResetScrollInput() { mMouseScrollInput = 0; }
	InputEvent PopInputEvent();
	CursorPosition GetCursorPosition() const;

	Graphics& Gfx();

private:
	void HandleResize(GLFWwindow* window, int width, int height) noexcept;
	void SetIcon();
	void AddScrollInput(double yOffset) { mMouseScrollInput += static_cast<float>(yOffset); }
	void PushInputEvent(InputEvent event);

	HWND hWnd;
	std::unique_ptr<Graphics> pGfx;
	GLFWwindow* mGlfwWindow;
	std::atomic_bool active = true;
	float mMouseScrollInput = 0;
	std::queue<InputEvent> mInputEvents;
};

//macro for capturing file and line to exception
#define WND_EXCEPT(hr) Window::Exception(__FILE__, __LINE__, hr)
#define WND_EXCEPT_LASTERR() WND_EXCEPT(GetLastError())
