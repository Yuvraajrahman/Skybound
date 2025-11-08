#include "enemy.h"

#include <algorithm>
#include <cmath>

#include "player.h"

void UpdateEnemies(std::vector<Enemy>& enemies, Player& player, float dt)
{
    for (Enemy& enemy : enemies)
    {
        enemy.bounds.x += enemy.speed * static_cast<float>(enemy.direction) * dt;

        if (enemy.bounds.x < enemy.leftLimit)
        {
            enemy.bounds.x = enemy.leftLimit;
            enemy.direction = 1;
        }
        else if (enemy.bounds.x + enemy.bounds.width > enemy.rightLimit)
        {
            enemy.bounds.x = enemy.rightLimit - enemy.bounds.width;
            enemy.direction = -1;
        }

        if (player.invincibilityTimer <= 0.0f && CheckCollisionRecs(enemy.bounds, GetPlayerBounds(player)))
        {
            player.lives = std::max(0, player.lives - enemy.damage);
            player.invincibilityTimer = 1.0f;

            if (player.velocity.y < 0.0f)
            {
                player.velocity.y = 0.0f;
            }

            player.position.x = player.previousPosition.x;
            player.position.y = player.previousPosition.y;
            player.velocity.x = 0.0f;
            player.velocity.y = 0.0f;
            player.comboCount = 0;
            player.comboTimer = 0.0f;
        }
    }
}

