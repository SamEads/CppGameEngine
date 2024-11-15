#include "keyboard.h"
#include <raylib.h>

// Called once per frame to update controller data
void updateControllers(void)
{

}

namespace Keyboard
{
    bool isKeyDown(Key key)
    {
        return IsKeyDown(static_cast<int>(key));
    }
    bool isKeyPressed(Key key)
    {
        return IsKeyPressed(static_cast<int>(key));
    }
    bool isKeyReleased(Key key)
    {
        return IsKeyReleased(static_cast<int>(key));
    }
}