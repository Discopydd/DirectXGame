#include <Windows.h>
#include <ImGuiManager.h>
#include "WinApp.h"
#include "DirectXCommon.h"
#include "Input.h"
#include "TextureManager.h"
#include "SrvManager.h"
#include "SpriteCommon.h"
#include "Sprite.h"
#include "Object3dCommon.h"
#include "Object3d.h"
#include "ModelManager.h"
#include "MyMath.h"
#include "ParticleManager.h"
#include "ParticleEmitter.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
    // ----------------------------------------
    // 基本システム初期化
    // ----------------------------------------
    auto* winApp = new WinApp();
    winApp->Initialize();

    auto* dxCommon = new DirectXCommon();
    dxCommon->Initialize(winApp);

    auto* input = new Input();
    input->Initialize(winApp);

    auto* spriteCommon = new SpriteCommon();
    spriteCommon->Initialize(dxCommon);

    auto* srvManager = new SrvManager();
    srvManager->Initialize(dxCommon);

    TextureManager::GetInstance()->Initialize(dxCommon, srvManager);

    auto* imguimanager = new ImGuiManager();
    imguimanager->Initialize(winApp, dxCommon, srvManager);

    auto* object3dCommon = new Object3dCommon();
    object3dCommon->Initialize(dxCommon);

    ModelManager::GetInstants()->Initialize(dxCommon);

    // ----------------------------------------
    // カメラ
    // ----------------------------------------
    auto* camera = new Camera();
    camera->SetRotate({ 0, 0, 0 });
    camera->SetTranslate({ 0, 0, -10 });

    object3dCommon->SetDefaultCamera(camera);

    // ----------------------------------------
    // モデル読み込みと設定
    // ----------------------------------------
    ModelManager::GetInstants()->LoadModel("plane.obj");
    ModelManager::GetInstants()->LoadModel("axis.obj");

    auto* object3d = new Object3d();
    object3d->Initialize(object3dCommon);
    object3d->SetModel("plane.obj");
    object3d->SetCamera(camera);

    // ----------------------------------------
    // スプライト生成
    // ----------------------------------------
    std::string textureFilePath[] = { "Resources/monsterBall.png", "Resources/uvChecker.png" };
    std::vector<Sprite*> sprites;

    for (uint32_t i = 0; i < 1; ++i) {
        auto* sprite = new Sprite();
        sprite->Initialize(spriteCommon, textureFilePath[1]);
        sprite->SetPosition({ 200.0f * i, 0.0f });
        sprite->SetAnchorPoint({ 0.0f, 0.0f });
        sprite->SetIsFlipY(false);
        sprites.push_back(sprite);
    }
    //パーティクルマネージャ
	ParticleManager::GetInstance()->Initialize(dxCommon, srvManager,camera);
	ParticleManager::GetInstance()->CreateparticleGroup("particle", "resources/circle.png");
	//パーティクルエミッター
	ParticleEmitter* particleEmitter = new ParticleEmitter();
	particleEmitter->Initialize("particle");
    // ----------------------------------------
    // Transform 初期化
    // ----------------------------------------
    Vector2 rotation = { 0 };
    Transform transform = { {1.0f,1.0f,1.0f}, {0.0f,0.0f,0.0f}, {0.0f,0.0f,0.0f} };
    Transform transformModel = transform;
    bool useMonsterBall = false;

    // ----------------------------------------
    // メインループ
    // ----------------------------------------
    while (true) {
        if (winApp->ProcessMessage()) break;

        camera->Update();
        imguimanager->Begin();
        input->Update();

        for (auto* sprite : sprites) {
            sprite->Update();
        }
        //パーティクル更新
        particleEmitter->Update();
        ParticleManager::GetInstance()->Update();
        rotation += 0.01f;
        object3d->SetRotate({ 0.0f, rotation.x, 0.0f });
        object3d->Update();

#ifdef USE_IMGUI
        ImGui::Begin("Camera Controller");

        Vector3 camPos = camera->GetTransform().translate;
        Vector3 camRot = camera->GetTransform().rotate;
        float camPosArr[3] = { camPos.x, camPos.y, camPos.z };
        float camRotArr[3] = { camRot.x, camRot.y, camRot.z };

        if (ImGui::DragFloat3("Position", camPosArr, 0.1f)) {
            camera->SetTranslate({ camPosArr[0], camPosArr[1], camPosArr[2] });
        }
        if (ImGui::DragFloat3("Rotation", camRotArr, 0.1f)) {
            camera->SetRotate({ camRotArr[0], camRotArr[1], camRotArr[2] });
        }

        ImGui::End();

#endif

        imguimanager->End();

        dxCommon->Begin();
        srvManager->PreDraw();
        object3dCommon->CommonDraw();
        object3d->Draw();
        spriteCommon->CommonDraw();

        for (auto* sprite : sprites) {
            sprite->Draw();
        }
        //パーティクル描画
      /*  ParticleManager::GetInstance()->Draw();*/
        imguimanager->Draw();
        dxCommon->End();
    }

    // ----------------------------------------
    // 終了処理
    // ----------------------------------------
    dxCommon->Finalize();
    winApp->Finalize();
   

	ParticleManager::GetInstance()->Finalize();
    TextureManager::GetInstance()->Finalize();
    ModelManager::GetInstants()->Finalize();
    imguimanager->Finalize();

    delete camera;
    delete input;
    delete winApp;
    delete dxCommon;
    delete spriteCommon;
    delete srvManager;
    delete object3dCommon;
    delete object3d;
    delete imguimanager;
    delete particleEmitter;
    for (auto* sprite : sprites) {
        delete sprite;
    }

    return 0;
}
