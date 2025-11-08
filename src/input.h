#pragma once

#include "raylib.h"

struct InputState
{
    bool moveLeft{false};
    bool moveRight{false};
    bool jumpPressed{false};
    bool pausePressed{false};
    bool restartPressed{false};
    bool openSettings{false};
    bool toggleHighContrast{false};
    bool toggleLargeHud{false};
    bool toggleTimeTrial{false};
    bool cycleBindings{false};
};

struct KeyPair
{
    KeyboardKey primary{KEY_NULL};
    KeyboardKey secondary{KEY_NULL};
};

struct InputBindings
{
    KeyPair moveLeft{};
    KeyPair moveRight{};
    KeyPair jump{};
    KeyPair pause{};
    KeyPair restart{};
};

InputBindings MakeDefaultBindings();
InputBindings MakeAlternativeBindings();

InputState PollInputState(const InputBindings& bindings);
