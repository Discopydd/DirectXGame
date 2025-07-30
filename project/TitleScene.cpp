#include "TitleScene.h"
#include "SceneManager.h"
#include "scene/GameScene.h"
void TitleScene::Initialize() {
    winApp_ = new WinApp();
    winApp_->Initialize();

    dxCommon_ = new DirectXCommon();
    dxCommon_->Initialize(winApp_);

    input_ = new Input();
    input_->Initialize(winApp_);

    spriteCommon_ = new SpriteCommon();
    spriteCommon_->Initialize(dxCommon_);

    srvManager_ = new SrvManager();
    srvManager_->Initialize(dxCommon_);

    TextureManager::GetInstance()->Initialize(dxCommon_, srvManager_);




    std::string textureFilePath[] = { "Resources/monsterBall.png", "Resources/uvChecker.png" };
    for (uint32_t i = 0; i < 1; ++i) {
        Sprite* sprite = new Sprite();
        sprite->Initialize(spriteCommon_, textureFilePath[0]);
        sprite->SetPosition({ 200.0f * i, 0.0f });
        sprite->SetAnchorPoint({ 0.0f, 0.0f });
        sprite->SetIsFlipY(false);
        sprites_.push_back(sprite);
    }
}

void TitleScene::Update() {
    input_->Update();
    for (auto* sprite : sprites_) {
        sprite->Update();
    }
    if (input_->TriggerKey(DIK_RETURN)) {
        BaseScene* next = new GameScene();
        sceneManager_->SetNextScene(next);
    }
}

void TitleScene::Draw() {
    dxCommon_->Begin();
    srvManager_->PreDraw();
    spriteCommon_->CommonDraw();
    for (auto* sprite : sprites_) {
        sprite->Draw();
    }
    dxCommon_->End();
}

void TitleScene::Finalize() {
    dxCommon_->Finalize();
    winApp_->Finalize();
    TextureManager::GetInstance()->Finalize();

    delete input_;
    delete winApp_;
    delete dxCommon_;
    delete spriteCommon_;
    delete srvManager_;

    for (auto* sprite : sprites_) {
        delete sprite;
    }
}
