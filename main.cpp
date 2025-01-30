#include<Windows.h>
#include "WinApp.h"
#include "DirectXCommon.h"
#include "TextureManager.h"

#include "Input.h"

#include "SpriteCommon.h"
#include "Sprite.h"

#include "Object3dCommon.h"
#include "ModelManager.h"
#include "Object3d.h"

#include "TransformationMatrix.h"
#include "MyMath.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

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

    SpriteCommon* spriteCommon = nullptr;
    spriteCommon = new SpriteCommon;
    spriteCommon->Initialize(dxCommon);

    //テクスチャマネージャの初期化
	TextureManager::GetInstance()->Initialize(dxCommon);

    //3Dオブジェクトの初期化
	Object3dCommon* object3dCommon = nullptr;
	object3dCommon = new Object3dCommon;
	object3dCommon->Initialize(dxCommon);


    ModelManager::GetInstants()->Initialize(dxCommon);


#pragma region 球

    const uint32_t kSubdivision = 16; // 分割数
    const uint32_t numVertices = (kSubdivision + 1) * (kSubdivision + 1);
    const uint32_t numIndices = kSubdivision * kSubdivision * 6;
    Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource = dxCommon->CreateBufferResource(sizeof(VertexData) * numVertices);

    D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};
    vertexBufferView.BufferLocation = vertexResource->GetGPUVirtualAddress();
    vertexBufferView.SizeInBytes = sizeof(VertexData) * numVertices;
    vertexBufferView.StrideInBytes = sizeof(VertexData);
    //頂点リソースにデータを書き込む
    VertexData* vertexData = nullptr;
    // 書き込むためのアドレスを取得
    vertexResource->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));

    const float kLonEvery = 2 * (float)M_PI / (float)kSubdivision; // 经度分割1つの角度
    const float kLatEvery = (float)M_PI / (float)kSubdivision; // 纬度分割1つの角度

    uint32_t vertexIndex = 0;
    for (uint32_t latIndex = 0; latIndex <= kSubdivision; ++latIndex) {
        float lat = -(float)M_PI / 2.0f + kLatEvery * latIndex; // 現在の緯度
        for (uint32_t lonIndex = 0; lonIndex <= kSubdivision; ++lonIndex) {
            float lon = lonIndex * kLonEvery; // 現在の経度

            vertexData[vertexIndex].position.x = cos(lat) * cos(lon);
            vertexData[vertexIndex].position.y = sin(lat);
            vertexData[vertexIndex].position.z = cos(lat) * sin(lon);
            vertexData[vertexIndex].position.w = 1.0f;
            vertexData[vertexIndex].texcoord.x = (float)lonIndex / (float)kSubdivision;
            vertexData[vertexIndex].texcoord.y = 1.0f - (float)latIndex / (float)kSubdivision;
            vertexData[vertexIndex].normal.x = vertexData[vertexIndex].position.x;
            vertexData[vertexIndex].normal.y = vertexData[vertexIndex].position.y;
            vertexData[vertexIndex].normal.z = vertexData[vertexIndex].position.z;
            ++vertexIndex;
        }
    }
    

#pragma endregion

#pragma region モデル
    // モデル読み込み
    ModelManager::GetInstants()->LoadModel("plane.obj");
	ModelManager::GetInstants()->LoadModel("axis.obj");

	//3Dオブジェクトの初期化
	Object3d* object3d = new Object3d();
	object3d->Initialize(object3dCommon);
	object3d->SetModel("plane.obj");
    
	//3Dオブジェクトの初期化
	Object3d* object3d2nd = new Object3d();
	object3d2nd->Initialize(object3dCommon);
	object3d2nd->SetModel("axis.obj");
#pragma endregion
    

    std::string textureFilePath[2]{ "Resources/monsterBall.png" ,"Resources/uvChecker.png" };
    std::vector<Sprite*>sprites;
	for (uint32_t i = 0; i < 1; ++i) {
		Sprite* sprite = new Sprite();
		sprite->Initialize(spriteCommon, textureFilePath[1]);
		sprites.push_back(sprite);
	}
    int i = 0;
	for (Sprite* sprite : sprites) {
		Vector2 position = sprite->GetPosition();

		position.x = 200.0f * i;
		position.y = 0.0f;

		sprite->SetPosition(position);
		sprite->SetAnchorPoint(Vector2{ 0.0f,0.0f });
		sprite->SetIsFlipY(0);
		i++;
	}

	Vector2 rotation{ 0 };
    Transform transform = { {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f} ,{0.0f,0.0f,0.0f} };

	Transform transformModel = { {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f} ,{0.0f,0.0f,0.0f} };
    bool useMonsterBall = false;
    while (true) {
        if (winApp->ProcessMessage()) {
            //ゲームループを抜ける
            break;
        }
        input->Update();
        for (Sprite* sprite : sprites) {
            sprite->Update();
        }
        rotation += 0.03f;

        object3d->SetRotate(Vector3{ 0.0f, rotation.x, 0.0f });
        object3d->Update();

        object3d2nd->SetRotate(Vector3{ rotation.x, 0.0f, 0.0f });
        object3d2nd->Update();

        
		ImGui_ImplDX12_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
		ImGui::Begin("Settings");
        	

		
		ImGui::End();
		ImGui::Render();

        dxCommon->Begin();

        object3dCommon->CommonDraw();
        object3d->Draw();
        object3d2nd->Draw();

        spriteCommon->CommonDraw();

        for (Sprite* sprite : sprites) {
            sprite->Draw();
        }
        ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), dxCommon->GetCommandList());

        dxCommon->End();
    }
    ImGui_ImplDX12_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
    delete input;
    winApp->Finalize();
    TextureManager::GetInstance()->Finalize();
	ModelManager::GetInstants()->Finalize();
    delete winApp;
    delete dxCommon;
    for (Sprite* sprite : sprites) {
		delete sprite;
	}

    delete spriteCommon;

    delete object3dCommon;
	delete object3d;
    delete object3d2nd;

	return 0;
}