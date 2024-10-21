#pragma once
#include <Windows.h>
#include <cstdint>
#include "externals/imgui/imgui.h"
#include "externals/imgui/imgui_impl_win32.h"
class WinApp
{
private:
	HWND hwnd = nullptr;

	WNDCLASS wc{};

	public:
    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

	 static const int32_t kClientWidth = 1280;
     static const int32_t kClientHeight = 720;
public:
	//
	void Initialize();
	//更新
	void Update();

	HWND GetHwnd() const { return hwnd; }
	HINSTANCE GetHInstance()const { return wc.hInstance; }
};