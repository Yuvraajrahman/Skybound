#pragma once

#include <vector>

#include "raylib.h"

#include "player.h"
#include "platform.h"
#include "enemy.h"
#include "coin.h"
#include "input.h"

enum class GameState
{
    Menu,
    Playing,
    Paused,
    GameOver
};

class Game
{
public:
    Game();
    ~Game();

    void Run();

private:
    void Init();
    void Shutdown();
    void Update(float dt);
    void Draw() const;
    void DrawGameplay() const;
    void DrawMenu() const;
    void DrawPause() const;
    void DrawGameOver() const;
    void ResetLevel();
    void UpdateCamera();

    GameState state{GameState::Menu};
    Player player{};
    std::vector<Platform> platforms{};
    std::vector<Enemy> enemies{};
    std::vector<Coin> coins{};
    Camera2D camera{};
    float gravity{780.0f};
    int screenWidth{1600};
    int screenHeight{900};
    int currentLevel{1};
    InputState inputState{};
    Music backgroundMusic{};
    bool musicLoaded{false};
};


