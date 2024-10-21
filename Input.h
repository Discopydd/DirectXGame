#pragma once
#include <dinput.h>
#include <Windows.h>
#include <wrl.h>
#include<dinput.h>
#include"WinApp.h"
class Input {
public:
	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
public: 
	void Initialize(WinApp* winApp);


	void Update();

	bool PushKey(BYTE keyNumber);

	bool TriggerKey(BYTE keyNumber);
private:
	ComPtr<IDirectInputDevice8> keyboard;
	ComPtr<IDirectInput8> directInput;
	BYTE key[256] = {};
	BYTE keyPre[256] = {};
	WinApp* winApp_ = nullptr;
};