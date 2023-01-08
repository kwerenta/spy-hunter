#include "game.h"
#include "application.h"

void initializeGameState(GameState* state) {
	state->score = 0;
	state->haltScore = 0;

	state->position = SCREEN_WIDTH / 2;

	state->carsLeft = 3;

	state->time = 0;
	state->distance = 0;
	state->speed = 1.0;

	state->status = PLAYING;
}