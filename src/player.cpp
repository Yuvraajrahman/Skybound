#include "player.h"

#include <algorithm>

#include "input.h"
#include "platform.h"

Rectangle GetPlayerBounds(const Player& player)
{
    return {player.position.x, player.position.y, player.width, player.height};
}

void ResetPlayer(Player& player, Vector2 spawnPosition)
{
    player.position = spawnPosition;
    player.previousPosition = spawnPosition;
    player.velocity = {0.0f, 0.0f};
    player.grounded = false;
    player.invincibilityTimer = 0.0f;
    player.comboCount = 0;
    player.comboTimer = 0.0f;
}

void ApplyPlayerInput(Player& player, const InputState& input, float)
{
    player.velocity.x = 0.0f;

    if (input.moveLeft)
    {
        player.velocity.x -= player.speed;
    }

    if (input.moveRight)
    {
        player.velocity.x += player.speed;
    }

    if (player.grounded && input.jumpPressed)
    {
        player.velocity.y = -player.jumpStrength;
        player.grounded = false;
    }
}

void UpdatePlayerPhysics(Player& player, float gravity, float dt)
{
    player.previousPosition = player.position;
    player.velocity.y += gravity * dt;
    player.position.x += player.velocity.x * dt;
    player.position.y += player.velocity.y * dt;
    player.invincibilityTimer = std::max(0.0f, player.invincibilityTimer - dt);
    player.comboTimer = std::max(0.0f, player.comboTimer - dt);
}

void ResolvePlayerPlatforms(Player& player, const std::vector<Platform>& platforms)
{
    player.grounded = false;
    Rectangle bounds = GetPlayerBounds(player);

    for (const Platform& platform : platforms)
    {
        Rectangle target = platform.bounds;
        if (!CheckCollisionRecs(bounds, target))
        {
            continue;
        }

        const float previousBottom = player.previousPosition.y + player.height;
        const float previousTop = player.previousPosition.y;
        const float previousRight = player.previousPosition.x + player.width;
        const float previousLeft = player.previousPosition.x;
        const float platformTop = target.y;
        const float platformBottom = target.y + target.height;
        const float platformLeft = target.x;
        const float platformRight = target.x + target.width;

        if (previousBottom <= platformTop && player.velocity.y >= 0.0f)
        {
            player.position.y = platformTop - player.height;
            player.velocity.y = 0.0f;
            player.grounded = true;
        }
        else if (previousTop >= platformBottom)
        {
            player.position.y = platformBottom;
            player.velocity.y = 0.0f;
        }
        else if (previousRight <= platformLeft)
        {
            player.position.x = platformLeft - player.width;
            player.velocity.x = 0.0f;
        }
        else if (previousLeft >= platformRight)
        {
            player.position.x = platformRight;
            player.velocity.x = 0.0f;
        }

        bounds = GetPlayerBounds(player);
    }
}

