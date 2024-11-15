#pragma once

#include "../utility/vec.h"

void updateMouse(Vec2<float> mouseDelta);

namespace Mouse
{
    enum class Button
    {
        LEFT,
        RIGHT,
        MIDDLE
    };
    Vec2<float> getDelta();
    bool isButtonPressed(Button button);
    void enable();
    void disable();
}