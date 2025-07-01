#pragma once

class Framework {
public:
    virtual ~Framework() = default;

    virtual void Initialize() = 0;
    virtual void Update() = 0;
    virtual void Draw() = 0;
    virtual void Finalize() = 0;

    void Run();  // 主循环

protected:
    bool endRequest_ = false;
};
