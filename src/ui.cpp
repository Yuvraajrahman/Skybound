#include "ui.h"

#include <cmath>
#include <cstdio>
#include <string>

#include "player.h"
#include "game.h"

namespace
{
    std::string FormatTime(float seconds)
    {
        if (seconds < 0.0f || std::isinf(seconds))
        {
            return "--:--.--";
        }

        int wholeSeconds = static_cast<int>(seconds);
        int minutes = wholeSeconds / 60;
        int secs = wholeSeconds % 60;
        int hundredths = static_cast<int>((seconds - static_cast<float>(wholeSeconds)) * 100.0f);
        if (hundredths < 0)
        {
            hundredths = 0;
        }

        char buffer[32];
        std::snprintf(buffer, sizeof(buffer), "%02d:%02d.%02d", minutes, secs, hundredths);
        return buffer;
    }

    const char* WeatherLabel(WeatherType type)
    {
        switch (type)
        {
            case WeatherType::Clear: return "Clear";
            case WeatherType::Rain: return "Rain";
            case WeatherType::Windy: return "Windy";
            case WeatherType::Storm: return "Storm";
            default: return "Unknown";
        }
    }
}

void DrawHUD(const Player& player,
             int level,
             bool timeTrialMode,
             bool timeTrialActive,
             float timeTrialTimer,
             float bestTimeTrial,
             const AccessibilityOptions& accessibility,
             const AchievementState& achievements,
             const WeatherState& weather)
{
    const int baseFont = accessibility.largeHud ? 32 : 24;
    const int smallFont = accessibility.largeHud ? 24 : 18;
    int y = 20;

    DrawText(TextFormat("Score: %d", player.score), 20, y, baseFont, RAYWHITE);
    y += baseFont + 6;
    DrawText(TextFormat("Lives: %d", player.lives), 20, y, baseFont, RAYWHITE);
    y += baseFont + 6;
    DrawText(TextFormat("Level: %d", level), 20, y, baseFont, RAYWHITE);
    y += baseFont + 10;

    if (player.comboCount > 1)
    {
        DrawText(TextFormat("Combo x%d!", player.comboCount), 20, y, smallFont + 4, GOLD);
        y += smallFont + 10;
    }

    DrawText(TextFormat("Best Combo: x%d", player.bestCombo), 20, y, smallFont, LIGHTGRAY);
    y += smallFont + 8;

    if (timeTrialMode)
    {
        const Color timerColor = timeTrialActive ? SKYBLUE : LIGHTGRAY;
        DrawText(TextFormat("Time Trial: %s", FormatTime(timeTrialTimer).c_str()), 20, y, smallFont + 2, timerColor);
        y += smallFont + 8;

        if (bestTimeTrial >= 0.0f)
        {
            DrawText(TextFormat("Best: %s", FormatTime(bestTimeTrial).c_str()), 20, y, smallFont, LIGHTGRAY);
            y += smallFont + 8;
        }
    }

    if (player.invincibilityTimer > 0.0f)
    {
        DrawText(TextFormat("Invincible %.1fs", player.invincibilityTimer), 20, y, smallFont, YELLOW);
    }

    if (achievements.notificationTimer > 0.0f && !achievements.lastUnlocked.empty())
    {
        const int textWidth = MeasureText(achievements.lastUnlocked.c_str(), baseFont);
        const int boxPadding = 20;
        const int boxWidth = textWidth + boxPadding * 2;
        const int boxHeight = baseFont + boxPadding;
        const int x = (GetScreenWidth() - boxWidth) / 2;
        const int boxY = 40;
        DrawRectangleRounded(Rectangle{static_cast<float>(x), static_cast<float>(boxY), static_cast<float>(boxWidth), static_cast<float>(boxHeight)}, 0.2f, 6, Color{0, 0, 0, 180});
        DrawText(achievements.lastUnlocked.c_str(), x + boxPadding, boxY + boxPadding / 2, baseFont, GOLD);
    }

    const int screenW = GetScreenWidth();
    int infoX = screenW - (accessibility.largeHud ? 300 : 240);
    int infoY = 20;
    DrawText(TextFormat("Weather: %s", WeatherLabel(weather.current)),
             infoX,
             infoY,
             baseFont - 2,
             RAYWHITE);
    infoY += baseFont;

    DrawText(TextFormat("Wind %+0.1f", weather.windCurrent),
             infoX,
             infoY,
             smallFont,
             LIGHTGRAY);
    infoY += smallFont + 6;

    if (weather.rainIntensity > 0.05f)
    {
        DrawText(TextFormat("Rain %d%%", static_cast<int>(weather.rainIntensity * 100.0f)),
                 infoX,
                 infoY,
                 smallFont,
                 SKYBLUE);
        infoY += smallFont + 6;
    }

    if (weather.lightningFlashTimer > 0.0f && weather.current == WeatherType::Storm)
    {
        DrawText("Lightning!",
                 infoX,
                 infoY,
                 smallFont,
                 YELLOW);
    }
}

void DrawMenuScreen(Vector2 screenSize, bool timeTrialMode)
{
    const char* title = "SkyBound";
    const int titleSize = 72;
    const int titleWidth = MeasureText(title, titleSize);
    DrawText(title,
             static_cast<int>(screenSize.x / 2 - titleWidth / 2),
             static_cast<int>(screenSize.y * 0.22f),
             titleSize,
             SKYBLUE);

    DrawText("Press Space/Enter to Start",
             static_cast<int>(screenSize.x * 0.28f),
             static_cast<int>(screenSize.y * 0.48f),
             28,
             RAYWHITE);

    DrawText(TextFormat("Time Trial Mode [T]: %s", timeTrialMode ? "Enabled" : "Disabled"),
             static_cast<int>(screenSize.x * 0.28f),
             static_cast<int>(screenSize.y * 0.55f),
             24,
             LIGHTGRAY);

    DrawText("Press O anytime for Accessibility & Controls",
             static_cast<int>(screenSize.x * 0.22f),
             static_cast<int>(screenSize.y * 0.65f),
             22,
             LIGHTGRAY);
}

void DrawPauseScreen(Vector2 screenSize)
{
    DrawRectangle(0, 0, static_cast<int>(screenSize.x), static_cast<int>(screenSize.y), Color{0, 0, 0, 120});

    const char* paused = "Paused";
    const int size = 48;
    const int width = MeasureText(paused, size);
    DrawText(paused,
             static_cast<int>(screenSize.x / 2 - width / 2),
             static_cast<int>(screenSize.y * 0.38f),
             size,
             RAYWHITE);

    DrawText("Press P to Resume",
             static_cast<int>(screenSize.x * 0.32f),
             static_cast<int>(screenSize.y * 0.5f),
             26,
             RAYWHITE);
}

void DrawGameOverScreen(Vector2 screenSize, int score, int bestCombo)
{
    DrawRectangle(0, 0, static_cast<int>(screenSize.x), static_cast<int>(screenSize.y), Color{0, 0, 0, 160});

    const char* gameOver = "Game Over";
    const int titleWidth = MeasureText(gameOver, 64);
    DrawText(gameOver,
             static_cast<int>(screenSize.x / 2 - titleWidth / 2),
             static_cast<int>(screenSize.y * 0.32f),
             64,
             RED);

    DrawText(TextFormat("Final Score: %d", score),
             static_cast<int>(screenSize.x * 0.3f),
             static_cast<int>(screenSize.y * 0.48f),
             30,
             RAYWHITE);

    DrawText(TextFormat("Best Combo: x%d", bestCombo),
             static_cast<int>(screenSize.x * 0.3f),
             static_cast<int>(screenSize.y * 0.54f),
             26,
             LIGHTGRAY);

    DrawText("Press R to Restart | Space to return to Menu",
             static_cast<int>(screenSize.x * 0.21f),
             static_cast<int>(screenSize.y * 0.66f),
             22,
             RAYWHITE);
}


