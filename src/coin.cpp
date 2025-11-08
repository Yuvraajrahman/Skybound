#include "coin.h"

#include <algorithm>

#include "player.h"

int CheckCoinCollection(std::vector<Coin>& coins, Player& player)
{
    const Rectangle playerBounds = GetPlayerBounds(player);
    int collectedThisFrame = 0;

    for (Coin& coin : coins)
    {
        if (coin.collected)
        {
            continue;
        }

        const Rectangle coinRect{coin.position.x - coin.radius, coin.position.y - coin.radius,
                                 coin.radius * 2.0f, coin.radius * 2.0f};

        if (CheckCollisionRecs(playerBounds, coinRect))
        {
            coin.collected = true;
            ++collectedThisFrame;
            player.comboCount += 1;
            player.comboTimer = player.comboWindow;
            player.bestCombo = std::max(player.bestCombo, player.comboCount);
            player.totalCoinsCollected += 1;

            const int comboBonus = player.comboCount > 1 ? (player.comboCount - 1) * 5 : 0;
            player.score += 10 + comboBonus;
        }
    }

    return collectedThisFrame;
}
