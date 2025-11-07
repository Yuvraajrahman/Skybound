#include "input.h"

#include "raylib.h"

InputState PollInputState()
{
    InputState state{};

    state.moveLeft = IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A);
    state.moveRight = IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D);
    state.jumpPressed = IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W);
    state.pausePressed = IsKeyPressed(KEY_P);
    state.restartPressed = IsKeyPressed(KEY_R);

#if defined(PLATFORM_ANDROID)
    if (IsGestureDetected(GESTURE_TAP))
    {
        Vector2 pos = GetTouchPosition(0);
        if (pos.x < GetScreenWidth() * 0.3f)
        {
            state.moveLeft = true;
        }
        else if (pos.x > GetScreenWidth() * 0.7f)
        {
            state.moveRight = true;
        }
        else
        {
            state.jumpPressed = true;
        }
    }
#endif

    return state;
}


