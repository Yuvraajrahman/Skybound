#include "input.h"

#include "raylib.h"

namespace
{
    bool IsKeyPairDown(const KeyPair& keys)
    {
        return (keys.primary != KEY_NULL && IsKeyDown(keys.primary))
            || (keys.secondary != KEY_NULL && IsKeyDown(keys.secondary));
    }

    bool IsKeyPairPressed(const KeyPair& keys)
    {
        return (keys.primary != KEY_NULL && IsKeyPressed(keys.primary))
            || (keys.secondary != KEY_NULL && IsKeyPressed(keys.secondary));
    }
}

InputBindings MakeDefaultBindings()
{
    InputBindings bindings{};
    bindings.moveLeft = {KEY_A, KEY_LEFT};
    bindings.moveRight = {KEY_D, KEY_RIGHT};
    bindings.jump = {KEY_SPACE, KEY_UP};
    bindings.pause = {KEY_P, KEY_ESCAPE};
    bindings.restart = {KEY_R, KEY_BACKSPACE};
    return bindings;
}

InputBindings MakeAlternativeBindings()
{
    InputBindings bindings{};
    bindings.moveLeft = {KEY_J, KEY_A};
    bindings.moveRight = {KEY_L, KEY_D};
    bindings.jump = {KEY_I, KEY_SPACE};
    bindings.pause = {KEY_O, KEY_P};
    bindings.restart = {KEY_U, KEY_R};
    return bindings;
}

InputState PollInputState(const InputBindings& bindings)
{
    InputState state{};

    state.moveLeft = IsKeyPairDown(bindings.moveLeft);
    state.moveRight = IsKeyPairDown(bindings.moveRight);
    state.jumpPressed = IsKeyPairPressed(bindings.jump);
    state.pausePressed = IsKeyPairPressed(bindings.pause);
    state.restartPressed = IsKeyPairPressed(bindings.restart);

    state.openSettings = IsKeyPressed(KEY_O);
    state.toggleHighContrast = IsKeyPressed(KEY_F3);
    state.toggleLargeHud = IsKeyPressed(KEY_F4);
    state.toggleTimeTrial = IsKeyPressed(KEY_T);
    state.cycleBindings = IsKeyPressed(KEY_F5);

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
