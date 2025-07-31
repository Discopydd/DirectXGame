#include "GameScene.h"

void GameScene::Initialize() {
    winApp_ = WinApp::GetInstance();
    dxCommon_ = DirectXCommon::GetInstance();
    input_ = Input::GetInstance();
    srvManager_ = SrvManager::GetInstance();

    spriteCommon_ = new SpriteCommon();
    spriteCommon_->Initialize(dxCommon_);


    TextureManager::GetInstance()->Initialize(dxCommon_, srvManager_);

    imguiManager_ = new ImGuiManager();
    imguiManager_->Initialize(winApp_, dxCommon_, srvManager_);

    object3dCommon_ = new Object3dCommon();
    object3dCommon_->Initialize(dxCommon_);

    ModelManager::GetInstants()->Initialize(dxCommon_);
    SoundManager* soundMgr = SoundManager::GetInstance();
    soundMgr->Initialize();
    soundMgr->LoadWav("fanfare", "resources/fanfare.wav");

    camera_ = new Camera();
    camera_->SetRotate({ 0, 0, 0 });
    camera_->SetTranslate({ 0, 0, -10 });
    object3dCommon_->SetDefaultCamera(camera_);

    ModelManager::GetInstants()->LoadModel("plane.obj");
    ModelManager::GetInstants()->LoadModel("axis.obj");

    object3d_ = new Object3d();
    object3d_->Initialize(object3dCommon_);
    object3d_->SetModel("plane.obj");
    object3d_->SetCamera(camera_);

    std::string textureFilePath[] = { "Resources/monsterBall.png", "Resources/uvChecker.png" };
    for (uint32_t i = 0; i < 1; ++i) {
        Sprite* sprite = new Sprite();
        sprite->Initialize(spriteCommon_, textureFilePath[1]);
        sprite->SetPosition({ 200.0f * i, 0.0f });
        sprite->SetAnchorPoint({ 0.0f, 0.0f });
        sprite->SetIsFlipY(false);
        sprites_.push_back(sprite);
    }

    ParticleManager::GetInstance()->Initialize(dxCommon_, srvManager_, camera_);
    ParticleManager::GetInstance()->CreateparticleGroup("particle", "resources/circle.png");
    particleEmitter_ = new ParticleEmitter();
    particleEmitter_->Initialize("particle");
}

void GameScene::Update() {
    camera_->Update();
    imguiManager_->Begin();
    input_->Update();

    for (auto* sprite : sprites_) {
        sprite->Update();
    }

    particleEmitter_->Update();
    ParticleManager::GetInstance()->Update();

    rotation_.x += 0.01f;
    object3d_->SetRotate({ 0.0f, rotation_.x, 0.0f });
    object3d_->Update();

    if (input_->TriggerKey(DIK_SPACE)) {
        SoundManager::GetInstance()->Play("fanfare", false, 1.0f);
    }

#ifdef USE_IMGUI
    ImGui::Begin("Camera Controller");
    Vector3 camPos = camera_->GetTransform().translate;
    Vector3 camRot = camera_->GetTransform().rotate;
    float camPosArr[3] = { camPos.x, camPos.y, camPos.z };
    float camRotArr[3] = { camRot.x, camRot.y, camRot.z };

    if (ImGui::DragFloat3("Position", camPosArr, 0.1f)) {
        camera_->SetTranslate({ camPosArr[0], camPosArr[1], camPosArr[2] });
    }
    if (ImGui::DragFloat3("Rotation", camRotArr, 0.1f)) {
        camera_->SetRotate({ camRotArr[0], camRotArr[1], camRotArr[2] });
    }
    ImGui::End();
#endif

    imguiManager_->End();
}

void GameScene::Draw() {
    dxCommon_->Begin();
    srvManager_->PreDraw();
    object3dCommon_->CommonDraw();
    object3d_->Draw();
    spriteCommon_->CommonDraw();
    for (auto* sprite : sprites_) {
        sprite->Draw();
    }
    // ParticleManager::GetInstance()->Draw();
    imguiManager_->Draw();
    dxCommon_->End();
}

void GameScene::Finalize() {
    ParticleManager::GetInstance()->Finalize();
    TextureManager::GetInstance()->Finalize();
    ModelManager::GetInstants()->Finalize();
    imguiManager_->Finalize();

    delete camera_;
    delete spriteCommon_;
    delete object3dCommon_;
    delete object3d_;
    delete imguiManager_;
    delete particleEmitter_;

    for (auto* sprite : sprites_) {
        delete sprite;
    }
}
