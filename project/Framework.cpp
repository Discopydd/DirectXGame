#include "Framework.h"
void Framework::Initialize() {
    sceneManager_ = new SceneManager();
}
void Framework::Run() {
    Initialize();

    while (!endRequest_) {
        Update();
        Draw();
    }

    Finalize();
}
void Framework::Update() {
    sceneManager_->Update();
}

void Framework::Draw() {
    sceneManager_->Draw();
}
void Framework::Finalize() {
    delete sceneManager_;
    sceneManager_ = nullptr;
}