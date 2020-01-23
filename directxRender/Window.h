#pragma once

#include "EasyWin.h"

class Window
{
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
	Window(const char* name, int width = 640, int height = 480) noexcept;
	~Window();
	Window(const Window&) = delete;
	Window& operator=(const Window&) = delete;
private:
	static LRESULT CALLBACK HandleMsgSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK DeliverMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	LRESULT CALLBACK HandleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	int width;
	int height;
	HWND hWnd;
};

