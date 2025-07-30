// SceneManager.h
#pragma once
#include "IScene.h"
#include <memory>

class SceneManager {
public:
    void ChangeScene(std::unique_ptr<IScene> newScene);
    void Update(bool& endRequest);
    void Draw();
    void Finalize();

private:
    std::unique_ptr<IScene> currentScene_;
};
