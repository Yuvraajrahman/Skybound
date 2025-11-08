#include "game.h"

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <limits>

#include "ui.h"

namespace
{
    constexpr Color BACKGROUND_COLOR{20, 24, 43, 255};
    constexpr Color PLATFORM_COLOR{115, 144, 114, 255};
    constexpr Color PLAYER_COLOR{221, 161, 94, 255};
    constexpr Color ENEMY_COLOR{191, 97, 106, 255};
    constexpr Color COIN_COLOR{229, 192, 123, 255};

    constexpr Color HIGH_CONTRAST_BG{10, 10, 10, 255};
    constexpr Color HIGH_CONTRAST_PLATFORM{225, 225, 225, 255};
    constexpr Color HIGH_CONTRAST_PLAYER{255, 230, 0, 255};
    constexpr Color HIGH_CONTRAST_ENEMY{255, 64, 64, 255};
    constexpr Color HIGH_CONTRAST_COIN{255, 200, 0, 255};

    constexpr float ACHIEVEMENT_DISPLAY_TIME = 3.5f;
    constexpr float FIXED_STEP = 1.0f / 120.0f;
}

Game::Game()
{
    bestTimeTrial = std::numeric_limits<float>::infinity();
}

Game::~Game()
{
    Shutdown();
}

void Game::Init()
{
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(screenWidth, screenHeight, "SkyBound");
    InitAudioDevice();
    SetTargetFPS(60);

    inputBindings = MakeDefaultBindings();
    InitParallax();
    UpdateParallaxPalette();

    camera.target = {0.0f, 0.0f};
    camera.offset = {static_cast<float>(screenWidth) / 2.0f, static_cast<float>(screenHeight) / 2.0f};
    camera.zoom = 1.0f;
    camera.rotation = 0.0f;

    ResetLevel();

    musicLoaded = false;
}

void Game::Shutdown()
{
    if (musicLoaded)
    {
        StopMusicStream(backgroundMusic);
        UnloadMusicStream(backgroundMusic);
        musicLoaded = false;
    }

    if (IsAudioDeviceReady())
    {
        CloseAudioDevice();
    }

    if (IsWindowReady())
    {
        CloseWindow();
    }
}

void Game::Run()
{
    Init();

    float timeAccumulator = 0.0f;

    while (!WindowShouldClose())
    {
        const float dt = GetFrameTime();
        timeAccumulator += dt;

        inputState = PollInputState(inputBindings);

        while (timeAccumulator >= FIXED_STEP)
        {
            Update(FIXED_STEP);
            timeAccumulator -= FIXED_STEP;
        }

        UpdateCamera();
        Draw();

        if (musicLoaded)
        {
            UpdateMusicStream(backgroundMusic);
        }
    }

    Shutdown();
}

void Game::Update(float dt)
{
    HandleInputToggles();

    switch (state)
    {
        case GameState::Menu:
        {
            if (inputState.jumpPressed || IsKeyPressed(KEY_ENTER))
            {
                state = GameState::Playing;
                ResetLevel();
                timeTrialActive = timeTrialMode;
                timeTrialTimer = 0.0f;
            }
            break;
        }
        case GameState::Playing:
        {
            if (inputState.pausePressed)
            {
                state = GameState::Paused;
                break;
            }

            UpdatePlatforms(platforms, dt);

            ApplyPlayerInput(player, inputState, dt);
            UpdatePlayerPhysics(player, gravity, dt);
            ResolvePlayerPlatforms(player, platforms);

            UpdateEnemies(enemies, player, dt);
            const int coinsCollected = CheckCoinCollection(coins, player);
            UpdateAchievements(coinsCollected, dt);
            UpdateComboTimer();

            const bool allCollected = std::all_of(coins.begin(), coins.end(), [](const Coin& coin) { return coin.collected; });
            const bool newBestTime = UpdateTimeTrial(dt, allCollected);

            if (newBestTime)
            {
                achievements.timeTrialClearUnlocked = true;
                achievements.lastUnlocked = "Speedrunner: New time trial record!";
                achievements.notificationTimer = ACHIEVEMENT_DISPLAY_TIME;
            }

            if (allCollected)
            {
                currentLevel += 1;
                player.lives = std::min(player.lives + 1, 5);
                ResetLevel();
                timeTrialActive = timeTrialMode;
                timeTrialTimer = 0.0f;
            }

            if (player.lives <= 0)
            {
                state = GameState::GameOver;
            }

            break;
        }
        case GameState::Paused:
        {
            if (inputState.pausePressed)
            {
                state = GameState::Playing;
            }
            break;
        }
        case GameState::GameOver:
        {
            if (inputState.restartPressed)
            {
                currentLevel = 1;
                ResetLevel();
                state = GameState::Playing;
                timeTrialActive = timeTrialMode;
                timeTrialTimer = 0.0f;
            }
            else if (inputState.jumpPressed)
            {
                state = GameState::Menu;
            }
            break;
        }
    }
}

void Game::Draw() const
{
    BeginDrawing();
    ClearBackground(accessibility.highContrast ? HIGH_CONTRAST_BG : BACKGROUND_COLOR);

    switch (state)
    {
        case GameState::Menu:
            DrawMenu();
            break;
        case GameState::Playing:
            DrawGameplay();
            break;
        case GameState::Paused:
            DrawGameplay();
            DrawPause();
            break;
        case GameState::GameOver:
            DrawGameplay();
            DrawGameOver();
            break;
    }

    if (showSettingsOverlay)
    {
        DrawSettingsOverlay();
    }

    EndDrawing();
}

void Game::DrawGameplay() const
{
    DrawBackground();

    BeginMode2D(camera);

    for (const Platform& platform : platforms)
    {
        const Color color = accessibility.highContrast ? HIGH_CONTRAST_PLATFORM : PLATFORM_COLOR;
        DrawRectangleRec(platform.bounds, color);
    }

    for (const Coin& coin : coins)
    {
        if (!coin.collected)
        {
            const Color color = accessibility.highContrast ? HIGH_CONTRAST_COIN : COIN_COLOR;
            DrawCircleV(coin.position, coin.radius, color);
        }
    }

    for (const Enemy& enemy : enemies)
    {
        const Color color = accessibility.highContrast ? HIGH_CONTRAST_ENEMY : ENEMY_COLOR;
        DrawRectangleRec(enemy.bounds, color);
    }

    const Color playerColor = accessibility.highContrast ? HIGH_CONTRAST_PLAYER : PLAYER_COLOR;
    DrawRectangleV(player.position, {player.width, player.height}, playerColor);

    EndMode2D();

    DrawHUD(player,
            currentLevel,
            timeTrialMode,
            timeTrialActive,
            timeTrialTimer,
            hasBestTime ? bestTimeTrial : -1.0f,
            accessibility,
            achievements);
}

void Game::DrawMenu() const
{
    DrawMenuScreen({static_cast<float>(screenWidth), static_cast<float>(screenHeight)}, timeTrialMode);
}

void Game::DrawPause() const
{
    DrawPauseScreen({static_cast<float>(screenWidth), static_cast<float>(screenHeight)});
}

void Game::DrawGameOver() const
{
    DrawGameOverScreen({static_cast<float>(screenWidth), static_cast<float>(screenHeight)}, player.score, player.bestCombo);
}

void Game::DrawSettingsOverlay() const
{
    const int margin = 48;
    const Rectangle panel{static_cast<float>(margin),
                          static_cast<float>(margin),
                          static_cast<float>(screenWidth - margin * 2),
                          static_cast<float>(screenHeight - margin * 2)};

    DrawRectangleRounded(panel, 0.08f, 12, Color{0, 0, 0, 160});

    int fontSize = accessibility.largeHud ? 36 : 28;
    int textY = margin + 24;
    DrawText("Accessibility & Controls", margin + 32, textY, fontSize + 8, RAYWHITE);
    textY += fontSize + 40;

    DrawText(TextFormat("High Contrast [F3]: %s", accessibility.highContrast ? "ON" : "OFF"),
             margin + 40,
             textY,
             fontSize,
             RAYWHITE);
    textY += fontSize + 20;

    DrawText(TextFormat("Large HUD [F4]: %s", accessibility.largeHud ? "ON" : "OFF"),
             margin + 40,
             textY,
             fontSize,
             RAYWHITE);
    textY += fontSize + 20;

    DrawText(TextFormat("Key Layout [F5]: %s", accessibility.alternativeBindings ? "ALT (J/L/I/O/U)" : "DEFAULT (A/D/SPACE/P/R)"),
             margin + 40,
             textY,
             fontSize,
             RAYWHITE);
    textY += fontSize + 20;

    DrawText(TextFormat("Time Trial Mode [T]: %s", timeTrialMode ? "ON" : "OFF"),
             margin + 40,
             textY,
             fontSize,
             RAYWHITE);
    textY += fontSize + 30;

    DrawText("Press O to close settings.", margin + 40, textY, fontSize - 8, LIGHTGRAY);
}

void Game::DrawBackground() const
{
    const float cameraX = camera.target.x;
    const float cameraY = camera.target.y;

    for (const ParallaxLayer& layer : parallaxLayers)
    {
        const float parallaxX = std::fmod(-cameraX * layer.scrollFactor, static_cast<float>(screenWidth));
        const float baseY = screenHeight - layer.height + layer.verticalOffset;

        for (int i = -1; i <= 1; ++i)
        {
            Rectangle rect{parallaxX + static_cast<float>(i * screenWidth),
                           baseY - cameraY * layer.scrollFactor * 0.1f,
                           static_cast<float>(screenWidth),
                           layer.height};
            DrawRectangleRec(rect, layer.color);
        }
    }
}

void Game::ResetLevel()
{
    platforms.clear();
    enemies.clear();
    coins.clear();

    if (currentLevel <= 1)
    {
        currentLevel = 1;
        player.score = 0;
        player.lives = 3;
        player.totalCoinsCollected = 0;
        player.bestCombo = 0;
        hasBestTime = false;
        bestTimeTrial = std::numeric_limits<float>::infinity();
    }

    const float groundHeight = 64.0f;
    platforms.push_back({Rectangle{ -400.0f, 400.0f, 1200.0f, groundHeight }, { -400.0f, 400.0f }, { -400.0f, 400.0f }, 0.0f, 0.0f, false});
    platforms.push_back({Rectangle{ 150.0f, 320.0f, 160.0f, 24.0f }, {150.0f, 320.0f}, {150.0f, 320.0f}, 0.0f, 0.0f, false});
    platforms.push_back({Rectangle{ 380.0f, 260.0f, 160.0f, 24.0f }, {380.0f, 260.0f}, {500.0f, 260.0f}, 2.5f, 0.0f, true});
    platforms.push_back({Rectangle{ 640.0f, 180.0f, 180.0f, 24.0f }, {640.0f, 180.0f}, {820.0f, 200.0f}, 3.0f, 0.0f, true});

    enemies.push_back({Rectangle{220.0f, 364.0f, 32.0f, 32.0f}, 50.0f, 150.0f, 310.0f, 1, 1});
    enemies.push_back({Rectangle{420.0f, 214.0f, 32.0f, 32.0f}, 70.0f, 380.0f, 520.0f, 1, -1});

    coins.push_back({Vector2{180.0f, 290.0f}, 12.0f, false});
    coins.push_back({Vector2{420.0f, 230.0f}, 12.0f, false});
    coins.push_back({Vector2{700.0f, 150.0f}, 12.0f, false});

    Vector2 spawnPoint{0.0f, 352.0f};
    ResetPlayer(player, spawnPoint);

    timeTrialActive = timeTrialMode;
    timeTrialTimer = 0.0f;

    if (musicLoaded)
    {
        StopMusicStream(backgroundMusic);
        PlayMusicStream(backgroundMusic);
    }
}

void Game::UpdateCamera()
{
    camera.target = {player.position.x + player.width * 0.5f, player.position.y + player.height * 0.5f};

    const float zoomMin = 0.6f;
    const float zoomMax = 1.2f;
    const float targetZoom = zoomMax - (player.velocity.y / 600.0f);
    camera.zoom = std::clamp(targetZoom, zoomMin, zoomMax);

    screenWidth = GetScreenWidth();
    screenHeight = GetScreenHeight();
    camera.offset = {static_cast<float>(screenWidth) / 2.0f, static_cast<float>(screenHeight) / 2.0f};
}

void Game::InitParallax()
{
    parallaxLayers = {
        ParallaxLayer{0.1f, Color{46, 52, 81, 255}, 260.0f, -80.0f},
        ParallaxLayer{0.2f, Color{66, 76, 114, 255}, 340.0f, -40.0f},
        ParallaxLayer{0.35f, Color{86, 99, 140, 255}, 420.0f, 0.0f},
    };
}

void Game::UpdateParallaxPalette()
{
    if (accessibility.highContrast)
    {
        for (std::size_t i = 0; i < parallaxLayers.size(); ++i)
        {
            unsigned char shade = static_cast<unsigned char>(50 + static_cast<int>(i) * 60);
            parallaxLayers[i].color = Color{shade, shade, shade, 255};
        }
    }
    else
    {
        InitParallax();
    }
}

void Game::HandleInputToggles()
{
    if (inputState.toggleHighContrast)
    {
        accessibility.highContrast = !accessibility.highContrast;
        UpdateParallaxPalette();
    }

    if (inputState.toggleLargeHud)
    {
        accessibility.largeHud = !accessibility.largeHud;
    }

    if (inputState.cycleBindings)
    {
        accessibility.alternativeBindings = !accessibility.alternativeBindings;
        inputBindings = accessibility.alternativeBindings ? MakeAlternativeBindings() : MakeDefaultBindings();
    }

    if (inputState.openSettings)
    {
        showSettingsOverlay = !showSettingsOverlay;
    }

    if (inputState.toggleTimeTrial)
    {
        timeTrialMode = !timeTrialMode;
        if (!timeTrialMode)
        {
            timeTrialActive = false;
        }
        else
        {
            timeTrialActive = true;
            timeTrialTimer = 0.0f;
        }
    }
}

void Game::UpdateComboTimer()
{
    if (player.comboTimer <= 0.0f)
    {
        player.comboCount = 0;
    }
}

void Game::UpdateAchievements(int coinsCollected, float dt)
{
    auto notify = [this](const char* message)
    {
        achievements.lastUnlocked = message;
        achievements.notificationTimer = ACHIEVEMENT_DISPLAY_TIME;
    };

    if (coinsCollected > 0 && !achievements.firstCoinUnlocked)
    {
        achievements.firstCoinUnlocked = true;
        notify("Shiny Start: Collected your first coin!");
    }

    if (player.totalCoinsCollected >= 10 && !achievements.tenCoinsUnlocked)
    {
        achievements.tenCoinsUnlocked = true;
        notify("Treasure Hunter: 10 coins collected!");
    }

    if (player.bestCombo >= 5 && !achievements.comboFiveUnlocked)
    {
        achievements.comboFiveUnlocked = true;
        notify("Combo Master: 5x combo achieved!");
    }

    if (achievements.notificationTimer > 0.0f)
    {
        achievements.notificationTimer = std::max(0.0f, achievements.notificationTimer - dt);
    }
}

bool Game::UpdateTimeTrial(float dt, bool levelCompleted)
{
    if (!timeTrialMode)
    {
        return false;
    }

    if (timeTrialActive)
    {
        timeTrialTimer += dt;
    }

    if (!levelCompleted)
    {
        return false;
    }

    if (!timeTrialActive)
    {
        return false;
    }

    timeTrialActive = false;

    bool newRecord = false;
    if (!hasBestTime || timeTrialTimer < bestTimeTrial)
    {
        bestTimeTrial = timeTrialTimer;
        hasBestTime = true;
        newRecord = true;
    }

    return newRecord;
}

