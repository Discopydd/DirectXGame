#include "Framework.h"

void Framework::Run() {
    Initialize();

    while (!endRequest_) {
        Update();
        Draw();
    }

    Finalize();
}
