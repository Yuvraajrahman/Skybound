#pragma once

struct InputState
{
    bool moveLeft{false};
    bool moveRight{false};
    bool jumpPressed{false};
    bool pausePressed{false};
    bool restartPressed{false};
};

InputState PollInputState();


