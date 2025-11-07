#pragma once

#include "raylib.h"

struct Player;

void DrawHUD(const Player& player, int level);
void DrawMenuScreen(Vector2 screenSize);
void DrawPauseScreen(Vector2 screenSize);
void DrawGameOverScreen(Vector2 screenSize, int score);


