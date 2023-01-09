#pragma once

#include <stdio.h>

#include "application.h"
#include "game.h"
#include "draw.h"

void renderLegend(Application* app, GameState* state, char* buffer, int bgColor, int borderColor);

void renderPause(Application* app, char* buffer);
