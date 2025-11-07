#include "coin.h"

#include "player.h"

void CheckCoinCollection(std::vector<Coin>& coins, Player& player)
{
    const Rectangle playerBounds = GetPlayerBounds(player);

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
            player.score += 10;
        }
    }
}


