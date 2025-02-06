#include<Windows.h>
#include <sstream>
#include<fstream>
#include<string>
#include<format>
#include <wrl.h>
#include<random>
#include <algorithm>

#include"Struct.h"
#include"MyMath.h"
#include"DebugReporter.h"
#include"Input.h"
#include"WinApp.h"
#include <numbers>
#include "Logger.h"
#include "DirectXCommon.h"
#include "D3DResourceLeakChecker.h"


int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {


    WinApp* winApp = nullptr;
    DirectXCommon* dxCommon = nullptr;
	Input* input = nullptr;

    //Windowの生成
    winApp = new WinApp();
    winApp->Initialize();

    // DX初期化
	dxCommon = new DirectXCommon;
	dxCommon->Initialize(winApp);

	// 入力初期化
	input = new Input();
	input->Initialize(winApp);

    
    while (true) {
        if (winApp->ProcessMessage()) {
            //ゲームループを抜ける
            break;
        }
        input->Update();

        
		ImGui_ImplDX12_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
		ImGui::Begin("Settings");
        	

		
		ImGui::End();
		ImGui::Render();

        dxCommon->Begin();

       
        ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), dxCommon->GetCommandList().Get());

        dxCommon->End();
    }
    ImGui_ImplDX12_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
    delete input;
    winApp->Finalize();
   
    delete winApp;
    delete dxCommon;

	return 0;
}