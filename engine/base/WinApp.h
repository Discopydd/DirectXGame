#pragma once
#include <Windows.h>
#include <cstdint>
#include "externals/imgui/imgui.h"
#include "externals/imgui/imgui_impl_dx12.h"
#include "externals/imgui/imgui_impl_win32.h"
class WinApp {
private:
    HWND hwnd = nullptr;
    WNDCLASS wc{};

public:
    static constexpr int32_t kClientWidth = 1280;
    static constexpr int32_t kClientHeight = 720;

    // 初始化窗口
    void Initialize();
    // 处理
    bool ProcessMessage();
    // 终止
    void Finalize();

    HWND GetHwnd() const { return hwnd; }
    HINSTANCE GetHInstance() const { return wc.hInstance; }

private:
    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
};
