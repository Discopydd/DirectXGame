#pragma once

#include "Framework.h"
#include "SceneManager.h"

class MyGame : public Framework {
public:
    void Initialize() override;
    void Update() override;
    void Draw() override;
    void Finalize() override;

private:
    SceneManager sceneManager_;
};
