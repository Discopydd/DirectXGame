#include "MyGame.h"
#include "scene/GameScene.h"

void MyGame::Initialize() {
    sceneManager_.ChangeScene(std::make_unique<GameScene>());
}

void MyGame::Update() {
    sceneManager_.Update(endRequest_);
}

void MyGame::Draw() {
    sceneManager_.Draw();
}

void MyGame::Finalize() {
    sceneManager_.Finalize();
}