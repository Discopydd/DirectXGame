#include<Windows.h>
#include <sstream>
#include<fstream>
#include<string>
#include<format>
#include <wrl.h>
#include<random>
#include <algorithm>
#include <list>

#include"Struct.h"
#include"MyMath.h"
#include"DebugReporter.h"
#include"Input.h"
#include"WinApp.h"
#include <numbers>
#include "Logger.h"
#include "DirectXCommon.h"
#include "D3DResourceLeakChecker.h"
#include "SpriteCommon.h"
#include "Sprite.h"
#include "TextureManager.h"


int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

    D3DResourceLeakChecker leakCheck;

    CoInitializeEx(0, COINIT_MULTITHREADED);

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

    SpriteCommon* spriteCommon = nullptr;
    spriteCommon = new SpriteCommon;
    spriteCommon->Initialize(dxCommon);




   TextureManager::GetInstance()->Initialize(dxCommon);
	TextureManager::GetInstance()->LoadTexture("Resources/uvChecker.png");
	TextureManager::GetInstance()->LoadTexture("Resources/monsterBall.png");

	//Sprite初期化
	std::vector<Sprite*>sprites;
	for (uint32_t i = 0; i < 2; ++i) {
		Sprite* sprite = new Sprite();
		sprite->Initialize(spriteCommon, "Resources/uvChecker.png");
		sprite->SetPosition({ 100.0f * i,50.0f });
		sprite->SetSize({ 100.0f,100.0f });
		sprites.push_back(sprite);
	}
	sprites[0]->SetTexture("Resources/uvChecker.png");
	sprites[1]->SettextureSize({ 200.0f,200.0f });
	sprites[1]->SetTexture("Resources/monsterBall.png");

#pragma endregion 


    while (true)
    {
        if (winApp->ProcessMessage()) {
            break;
        }
        else {
            input->Update();
            if (input->TriggerKey(DIK_0)) {
                OutputDebugStringA("Hit 0\n");
            }
            // ImGuiの新しいフレームを開始する
            //ImGui_ImplDX12_NewFrame();
            //ImGui_ImplWin32_NewFrame();
            //ImGui::NewFrame();

            // ImGuiウィンドウの設定
            //ImGui::Begin("Window");

          
      for (size_t i = 0; i < sprites.size(); ++i) {
			Sprite* sprite = sprites[i];
			sprite->Update();
		}

		dxCommon->Begin();

		//Spriteの描画準備
		spriteCommon->CommonDraw();


		for (Sprite* sprite : sprites) {
			//sprite描画処理
			sprite->Draw();
		}
            //ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), dxCommon->GetCommandList());
            dxCommon->End();
        }
    }

    ImGui_ImplDX12_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    delete input;
    winApp->Finalize();
    delete winApp;
    delete dxCommon;
    //Sprite
	for (Sprite* sprite : sprites) {
		delete sprite;
	}
    delete spriteCommon;
    return 0;
}