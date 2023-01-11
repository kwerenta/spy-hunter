#pragma once

#include <stdio.h>
#include <string.h>

#include "application.h"
#include "game.h"
#include "draw.h"

void renderLegend(Application* app, GameState* state, char* buffer, int bgColor, int borderColor);
void renderFunctionalities(Application* app, char* buffer, int bgColor, int borderColor);
void renderPause(Application* app, char* buffer);
void renderBackground(Application* app, int backgroundOffset);
void renderRoad(Application* app, GameState* state, int backgroundOffset, int color);
void renderGameSaveSelection(Application* app, char* buffer, int bgColor, int borderColor, int selectionColor, int selection);
void renderGameOver(Application* app, GameState* state, char* buffer, int bgColor, int borderColor, int selectionColor, int selection);
void renderScoreboard(Application* app, GameState* state, char* buffer, int bgColor, int borderColor);