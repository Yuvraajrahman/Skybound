#pragma once

#include <vector>

#include "raylib.h"

struct Enemy
{
    Rectangle bounds{};
    float speed{80.0f};
    float leftLimit{0.0f};
    float rightLimit{0.0f};
    int damage{1};
    int direction{1};
};

void UpdateEnemies(std::vector<Enemy>& enemies, struct Player& player, float dt);

