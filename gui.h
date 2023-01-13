#pragma once

#include <stdio.h>
#include <string.h>

#include "application.h"
#include "game.h"
#include "draw.h"

void renderLegend(Application* app, GameState* state, char* buffer);
void renderFunctionalities(Application* app, char* buffer);
void renderPause(Application* app, char* buffer);
void renderBackground(Application* app, int backgroundOffset);
void renderRoad(Application* app, GameState* state);
void renderAI(Application* app, GameState* state);
void renderGameSaveSelection(Application* app, char* buffer, int selection);
void renderGameOver(Application* app, GameState* state, char* buffer, int selection);
void renderScoreboard(Application* app, GameState* state, char* buffer);