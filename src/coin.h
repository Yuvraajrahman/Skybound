#pragma once

#include <vector>

#include "raylib.h"

struct Coin
{
    Vector2 position{};
    float radius{12.0f};
    bool collected{false};
};

int CheckCoinCollection(std::vector<Coin>& coins, struct Player& player);


