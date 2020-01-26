#pragma once

#include "EasyWin.h"
#include "ExtendedException.h"
#include "Keyboard.h"
#include "Mouse.h"

#include <string>
#include <optional>

class Window
{
public:
	class Exception : public ExtendedException
	{
	public:
		Exception(const char* file, int line, HRESULT hr);
		const char* what() const override;
		virtual const char* GetType() const noexcept;
		static std::string TranslateErrorCode(HRESULT hr);
		HRESULT GetErrorCode() const noexcept;
		std::string GetErrorString() const;
	private:
		HRESULT hr;
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
	std::optional<int> ProcessMessages();

	Keyboard keyboard;
	Mouse mouse;
private:
	static LRESULT CALLBACK HandleMsgSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK DeliverMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	LRESULT CALLBACK HandleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	bool PointsInside(LPARAM lParam) const noexcept;

	int width;
	int height;
	HWND hWnd;
};

//macro for capturing file and line to exception
#define WND_EXCEPT(hr) Window::Exception(__FILE__, __LINE__, hr)
#define WND_EXCEPT_LASTERR() WND_EXCEPT(GetLastError())
