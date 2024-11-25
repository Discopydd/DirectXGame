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


    Sprite* sprite = new Sprite();
    sprite->Initialize(spriteCommon);

    //Sprite初期化
	std::vector<Sprite*>sprites;
	for (uint32_t i = 0; i < 5; ++i) {
		Sprite* sprite = new Sprite();
		sprite->Initialize(spriteCommon);
		sprite->SetPosition({ 100.0f * i,0.0f });
		sprite->SetSize({ 50.0f,50.0f });
		sprites.push_back(sprite);
	}


    // Textureを読んで転送する
    DirectX::ScratchImage mipImages = dxCommon->LoadTexture("Resources/uvChecker.png");
    const DirectX::TexMetadata& metadata = mipImages.GetMetadata();
    Microsoft::WRL::ComPtr<ID3D12Resource> textureResource = dxCommon->CreateTextureResource(metadata);
    Microsoft::WRL::ComPtr<ID3D12Resource> intermediateResource = dxCommon->UploadTextureData(textureResource, mipImages);



    // metaDataを基にSRVの設定
    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Format = metadata.format;
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;//2Dテクスチャ
    srvDesc.Texture2D.MipLevels = static_cast<UINT>(metadata.mipLevels);


    //SRVを作成する DescriptorHeapの場所を決める
    D3D12_CPU_DESCRIPTOR_HANDLE textureSrvHandleCPU = dxCommon->GetSRVCPUDescriptorHandle(1);
    //D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU = dxCommon->GetSRVGPUDescriptorHandle(1);


    //textureSrvHandleCPU.ptr += dxCommon->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    //textureSrvHandleGPU.ptr += dxCommon->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    // SRVの設定
    dxCommon->GetDevice()->CreateShaderResourceView(textureResource.Get(), &srvDesc, textureSrvHandleCPU);

    Transform transformSprite{ {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} };
	Transform cameraTransform{ {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,-10.0f} };
	Transform uvTransformSprite{ {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} };
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

			//回転テスト
			float rotaion = sprite->GetRotation();
			rotaion += 0.01f;
			sprite->SetRotation(rotaion);

			//Spriteの更新
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