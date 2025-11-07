#pragma once

#include <vector>

#include "raylib.h"

struct Platform;
struct InputState;

struct Player
{
    Vector2 position{};
    Vector2 velocity{};
    Vector2 previousPosition{};
    float width{32.0f};
    float height{48.0f};
    bool grounded{false};
    int lives{3};
    int score{0};
    float speed{180.0f};
    float jumpStrength{320.0f};
    float invincibilityTimer{0.0f};
};

Rectangle GetPlayerBounds(const Player& player);
void ResetPlayer(Player& player, Vector2 spawnPosition);
void ApplyPlayerInput(Player& player, const InputState& input, float dt);
void UpdatePlayerPhysics(Player& player, float gravity, float dt);
void ResolvePlayerPlatforms(Player& player, const std::vector<Platform>& platforms);

