#pragma once

#include "utility/EasyWin.h"
#include "exception/HrException.h"
#include "controls/Keyboard.h"
#include "controls/Mouse.h"
#include "renderSystem/Graphics.h"

#include <string>
#include <optional>
#include <memory>
#include <functional>

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
private:
	// singleton for registration of window class
	class WindowClass
	{
	public:
		static const char *GetName() noexcept;
		static HINSTANCE GetInstance() noexcept;
	private:
		WindowClass() noexcept;
		~WindowClass();
		WindowClass(const WindowClass&) = delete;
		WindowClass& operator=(const WindowClass&) = delete;
		static constexpr const char* name = "dx render";
		static WindowClass windowClass;
		HINSTANCE hInst;
	};
public:
	Window(const char* name, int width = 640, int height = 480);
	~Window();
	Window(const Window&) = delete;
	Window& operator=(const Window&) = delete;
	void SetTitle(const std::string& title);
	int GetWidth() const noexcept;
	int GetHeight() const noexcept;
	bool Active() const;
	std::optional<int> ProcessMessages();
	void SetActivationHandler(handler_t handler);

	Mouse& GetMouse();
	Keyboard& GetKeyboard();
	Graphics& Gfx();

private:
	static LRESULT CALLBACK HandleMsgSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK DeliverMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	LRESULT CALLBACK HandleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	bool PointsInside(LPARAM lParam) const noexcept;
	void HandleResize(LPARAM lParam) noexcept;
	Mouse mouse;
	Keyboard keyboard;

	int width;
	int height;
	HWND hWnd;
	std::unique_ptr<Graphics> pGfx;
	bool active = false;
	std::optional<handler_t> onActiveChanged;
};

//macro for capturing file and line to exception
#define WND_EXCEPT(hr) Window::Exception(__FILE__, __LINE__, hr)
#define WND_EXCEPT_LASTERR() WND_EXCEPT(GetLastError())
