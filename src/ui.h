#pragma once

#include "raylib.h"

struct Player;
struct AccessibilityOptions;
struct AchievementState;

void DrawHUD(const Player& player,
             int level,
             bool timeTrialMode,
             bool timeTrialActive,
             float timeTrialTimer,
             float bestTimeTrial,
             const AccessibilityOptions& accessibility,
             const AchievementState& achievements);
void DrawMenuScreen(Vector2 screenSize, bool timeTrialMode);
void DrawPauseScreen(Vector2 screenSize);
void DrawGameOverScreen(Vector2 screenSize, int score, int bestCombo);


