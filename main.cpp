#include<Windows.h>
#include <sstream>
#include<fstream>
#include<string>
#include<format>
#include <wrl.h>
#include<random>
#include <algorithm>

#include"math/MyMath.h"
#include"math/Vector3.h"
#include"MaterialData.h"
#include"ModelData.h"
#include"DebugReporter.h"
#include"Input.h"
#include"WinApp.h"
#include <numbers>
#include "Logger.h"
#include "DirectXCommon.h"
#include "D3DResourceLeakChecker.h"
#include "DirectionalLight.h"
#include <corecrt_math_defines.h>
#include "SpriteCommon.h"
#include "Sprite.h"
#include "TextureManager.h"
#include "Object3dCommon.h"
#include "Object3d.h"
#include "ModelManager.h"


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

  

    // WVP用のリソースを作る。 Matrix4x41つ分のサイズを用意する
    Microsoft::WRL::ComPtr<ID3D12Resource> wvpResource = dxCommon->CreateBufferResource(sizeof(TransformationMatrix));

    // データを書き込む
    TransformationMatrix* wvpData = nullptr;
    wvpResource->Map(0, nullptr, reinterpret_cast<void**>(&wvpData));
    wvpData->World =  Math::MakeIdentity4x4(); // 単位行列を書きこんでおく
    wvpData->WVP =  Math::MakeIdentity4x4();

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


        dxCommon->Begin();

        object3dCommon->CommonDraw();
        object3d->Draw();
        object3d2nd->Draw();

        spriteCommon->CommonDraw();

        for (Sprite* sprite : sprites) {
            sprite->Draw();
        }

        dxCommon->End();
    }
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