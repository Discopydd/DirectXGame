// SceneManager.cpp
#include "SceneManager.h"

void SceneManager::ChangeScene(std::unique_ptr<IScene> newScene) {
    if (currentScene_) currentScene_->Finalize();
    currentScene_ = std::move(newScene);
    currentScene_->Initialize();
}

void SceneManager::Update(bool& endRequest) {
    if (currentScene_) currentScene_->Update(endRequest);
}

void SceneManager::Draw() {
    if (currentScene_) currentScene_->Draw();
}

void SceneManager::Finalize() {
    if (currentScene_) currentScene_->Finalize();
    currentScene_.reset();
}
