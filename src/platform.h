#pragma once

#include <vector>

#include "raylib.h"

struct Platform
{
    Rectangle bounds{};
    Vector2 startPosition{};
    Vector2 endPosition{};
    float travelTime{0.0f};
    float timer{0.0f};
    bool moving{false};
};

void UpdatePlatforms(std::vector<Platform>& platforms, float dt);


