#include "mouse.h"
#include <raylib.h>

namespace
{
    Vec2<float> delta { 0 };
}

namespace Mouse
{
    Vec2<float> getDelta()
    {
        return delta;
    }
    bool isButtonPressed(Button button)
    {
        return IsMouseButtonPressed((int)button);
    }
    void enable()
    {
        EnableCursor();
    }
    void disable()
    {
        DisableCursor();
    }
}

void updateMouse(Vec2<float> mouseDelta)
{
    delta = mouseDelta;
}