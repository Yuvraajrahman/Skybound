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

enum class WeatherType
{
    Clear,
    Rain,
    Windy,
    Storm
};

struct RainDrop
{
    Vector2 position{};
    float length{18.0f};
    float speed{600.0f};
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

struct WeatherState
{
    WeatherType current{WeatherType::Clear};
    float timeUntilChange{0.0f};
    float rainIntensity{0.0f};
    float baseWind{0.0f};
    float windVariance{0.0f};
    float windCurrent{0.0f};
    float windTarget{0.0f};
    float windChangeTimer{0.0f};
    float lightningCooldown{0.0f};
    float lightningFlashTimer{0.0f};
    std::vector<RainDrop> rainDrops{};
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
    void DrawWeather() const;
    void ResetLevel();
    void UpdateCamera();
    void InitParallax();
    void UpdateParallaxPalette();
    void InitWeather();
    void SetWeather(WeatherType type);
    void UpdateWeather(float dt);
    Vector2 GetWeatherForce() const;
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
    WeatherState weather{};
};


