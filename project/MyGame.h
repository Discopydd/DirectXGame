#pragma once

#include "Framework.h"
#include "scene/GameScene.h"
#include "TitleScene.h"
#include "BaseScene.h"

class MyGame : public Framework {
public:
    void Initialize() override;
    void Update() override;
    void Draw() override;
    void Finalize() override;

private:
    BaseScene* scene_ = nullptr;
};
