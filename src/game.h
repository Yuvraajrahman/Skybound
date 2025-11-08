#pragma once

#include <array>
#include <string>
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

struct ParallaxLayer
{
    float scrollFactor{0.0f};
    Color color{};
    float height{0.0f};
    float verticalOffset{0.0f};
};

struct AccessibilityOptions
{
    bool highContrast{false};
    bool largeHud{false};
    bool alternativeBindings{false};
};

struct AchievementState
{
    bool firstCoinUnlocked{false};
    bool tenCoinsUnlocked{false};
    bool comboFiveUnlocked{false};
    bool timeTrialClearUnlocked{false};
    std::string lastUnlocked{};
    float notificationTimer{0.0f};
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
    void DrawSettingsOverlay() const;
    void DrawBackground() const;
    void ResetLevel();
    void UpdateCamera();
    void InitParallax();
    void UpdateParallaxPalette();
    void HandleInputToggles();
    void UpdateComboTimer();
    void UpdateAchievements(int coinsCollected, float dt);
    bool UpdateTimeTrial(float dt, bool levelCompleted);

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
    InputBindings inputBindings{MakeDefaultBindings()};
    Music backgroundMusic{};
    bool musicLoaded{false};
    std::array<ParallaxLayer, 3> parallaxLayers{};
    AccessibilityOptions accessibility{};
    bool showSettingsOverlay{false};
    AchievementState achievements{};
    bool timeTrialMode{false};
    bool timeTrialActive{false};
    float timeTrialTimer{0.0f};
    float bestTimeTrial{0.0f};
    bool hasBestTime{false};
};


