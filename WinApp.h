#pragma once
#include <Windows.h>
#include "externals/imgui/imgui.h"
#include "externals/imgui/imgui_impl_win32.h"
class WinApp
{
	public:
    HWND hwnd_ = nullptr;  // 窗口句柄
    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
public:
	//
	void Initialize();
	//更新
	void Update();

};