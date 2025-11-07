#include "ui.h"

#include <string>

#include "player.h"

void DrawHUD(const Player& player, int level)
{
    const std::string scoreText = "Score: " + std::to_string(player.score);
    const std::string livesText = "Lives: " + std::to_string(player.lives);
    const std::string levelText = "Level: " + std::to_string(level);

    DrawText(scoreText.c_str(), 20, 20, 24, RAYWHITE);
    DrawText(livesText.c_str(), 20, 52, 24, RAYWHITE);
    DrawText(levelText.c_str(), 20, 84, 24, RAYWHITE);

    if (player.invincibilityTimer > 0.0f)
    {
        const std::string invText = "Invincible " + std::to_string(static_cast<int>(player.invincibilityTimer * 100.0f) / 100);
        DrawText(invText.c_str(), 20, 116, 20, YELLOW);
    }
}

void DrawMenuScreen(Vector2 screenSize)
{
    const char* title = "SkyBound";
    const char* prompt = "Press Space or Enter to Start";

    const int titleWidth = MeasureText(title, 72);
    DrawText(title,
             static_cast<int>(screenSize.x / 2 - titleWidth / 2),
             static_cast<int>(screenSize.y * 0.25f),
             72,
             SKYBLUE);

    const int promptWidth = MeasureText(prompt, 28);
    DrawText(prompt,
             static_cast<int>(screenSize.x / 2 - promptWidth / 2),
             static_cast<int>(screenSize.y * 0.55f),
             28,
             RAYWHITE);
}

void DrawPauseScreen(Vector2 screenSize)
{
    const char* paused = "Paused";
    const int textWidth = MeasureText(paused, 48);

    DrawRectangle(0, 0, static_cast<int>(screenSize.x), static_cast<int>(screenSize.y), Color{0, 0, 0, 120});
    DrawText(paused,
             static_cast<int>(screenSize.x / 2 - textWidth / 2),
             static_cast<int>(screenSize.y * 0.4f),
             48,
             RAYWHITE);

    const char* instruction = "Press P to Resume";
    const int instrWidth = MeasureText(instruction, 24);
    DrawText(instruction,
             static_cast<int>(screenSize.x / 2 - instrWidth / 2),
             static_cast<int>(screenSize.y * 0.55f),
             24,
             RAYWHITE);
}

void DrawGameOverScreen(Vector2 screenSize, int score)
{
    DrawRectangle(0, 0, static_cast<int>(screenSize.x), static_cast<int>(screenSize.y), Color{0, 0, 0, 160});

    const char* gameOver = "Game Over";
    const int titleWidth = MeasureText(gameOver, 64);
    DrawText(gameOver,
             static_cast<int>(screenSize.x / 2 - titleWidth / 2),
             static_cast<int>(screenSize.y * 0.35f),
             64,
             RED);

    const std::string scoreText = "Final Score: " + std::to_string(score);
    const int scoreWidth = MeasureText(scoreText.c_str(), 28);
    DrawText(scoreText.c_str(),
             static_cast<int>(screenSize.x / 2 - scoreWidth / 2),
             static_cast<int>(screenSize.y * 0.52f),
             28,
             RAYWHITE);

    const char* restartPrompt = "Press R to Restart or Space to Return to Menu";
    const int promptWidth = MeasureText(restartPrompt, 20);
    DrawText(restartPrompt,
             static_cast<int>(screenSize.x / 2 - promptWidth / 2),
             static_cast<int>(screenSize.y * 0.65f),
             20,
             RAYWHITE);
}


