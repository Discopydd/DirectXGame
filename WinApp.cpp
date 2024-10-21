#include"WinApp.h"
#include "externals/imgui/imgui.h"
#include "externals/imgui/imgui_impl_win32.cpp"


LRESULT WinApp::WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	  // ImGui 関連のメッセージ処理
    if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam)) {
        return true;
    }

    // メッセージに応じてゲーム固有の処理を行う
    switch (msg) {
    // ウィンドウが破棄された場合
    case WM_DESTROY:
        // OSに対して、アプリの終了を伝える
        PostQuitMessage(0);
        return 0;
    }

    // 標準のメッセージ処理を行う
    return DefWindowProc(hwnd, msg, wparam, lparam);
}

void WinApp::Initialize()
{
	HRESULT hr = CoInitializeEx(0, COINIT_MULTITHREADED);

WNDCLASS wc{};
wc.lpfnWndProc = WindowProc;
wc.lpszClassName = L"CG2WindowClass";
wc.hInstance = GetModuleHandle(nullptr);
wc.hCursor = LoadCursor(nullptr, IDC_ARROW);

RegisterClass(&wc);

const int32_t kClientWidth = 1280;
const int32_t kClientHeight = 720;

RECT wrc = { 0, 0, kClientWidth, kClientHeight };
AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false);

HWND hwnd = CreateWindow(
    wc.lpszClassName,
    L"CG2",
    WS_OVERLAPPEDWINDOW,
    CW_USEDEFAULT,
    CW_USEDEFAULT,
    wrc.right - wrc.left,
    wrc.bottom - wrc.top,
    nullptr,
    nullptr,
    wc.hInstance,
    nullptr
);

ShowWindow(hwnd, SW_SHOW);


}

void WinApp::Update()
{
}
