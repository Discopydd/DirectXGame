#pragma once
#include "SceneManager.h"
class Framework {
public:
    virtual ~Framework() = default;

    virtual void Initialize();
    virtual void Update();
    virtual void Draw();
    virtual void Finalize();


    void Run();  // 主循环

protected:
    bool endRequest_ = false;
    SceneManager* sceneManager_ = nullptr;
};
