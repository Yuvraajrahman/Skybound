#include "game.h"

#include <algorithm>
#include <cmath>

#include "ui.h"

namespace
{
    constexpr Color BACKGROUND_COLOR{20, 24, 43, 255};
    constexpr Color PLATFORM_COLOR{115, 144, 114, 255};
    constexpr Color PLAYER_COLOR{221, 161, 94, 255};
    constexpr Color ENEMY_COLOR{191, 97, 106, 255};
    constexpr Color COIN_COLOR{229, 192, 123, 255};

    constexpr float FIXED_STEP = 1.0f / 120.0f;
}

Game::Game() = default;

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

    camera.target = {0.0f, 0.0f};
    camera.offset = {static_cast<float>(screenWidth) / 2.0f, static_cast<float>(screenHeight) / 2.0f};
    camera.zoom = 1.0f;
    camera.rotation = 0.0f;

    ResetLevel();

    // Placeholder for background music loading if assets are added later.
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

        inputState = PollInputState();

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
    switch (state)
    {
        case GameState::Menu:
        {
            if (inputState.jumpPressed || IsKeyPressed(KEY_ENTER))
            {
                state = GameState::Playing;
                ResetLevel();
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
            CheckCoinCollection(coins, player);

            bool allCollected = std::all_of(coins.begin(), coins.end(), [](const Coin& coin) { return coin.collected; });
            if (allCollected)
            {
                currentLevel += 1;
                player.lives = std::min(player.lives + 1, 5);
                ResetLevel();
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
    ClearBackground(BACKGROUND_COLOR);

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

    EndDrawing();
}

void Game::DrawGameplay() const
{
    BeginMode2D(camera);

    for (const Platform& platform : platforms)
    {
        DrawRectangleRec(platform.bounds, PLATFORM_COLOR);
    }

    for (const Coin& coin : coins)
    {
        if (!coin.collected)
        {
            DrawCircleV(coin.position, coin.radius, COIN_COLOR);
        }
    }

    for (const Enemy& enemy : enemies)
    {
        DrawRectangleRec(enemy.bounds, ENEMY_COLOR);
    }

    DrawRectangleV(player.position, {player.width, player.height}, PLAYER_COLOR);

    EndMode2D();

    DrawHUD(player, currentLevel);
}

void Game::DrawMenu() const
{
    DrawMenuScreen({static_cast<float>(screenWidth), static_cast<float>(screenHeight)});
}

void Game::DrawPause() const
{
    DrawPauseScreen({static_cast<float>(screenWidth), static_cast<float>(screenHeight)});
}

void Game::DrawGameOver() const
{
    DrawGameOverScreen({static_cast<float>(screenWidth), static_cast<float>(screenHeight)}, player.score);
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

