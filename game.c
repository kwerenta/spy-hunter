#include "game.h"

void initializeGameState(GameState* state) {
	state->score = 0;
	state->haltScore = 0;
	state->carsLeft = 3;

	state->position = 0;
	state->time = 0;
	state->distance = 0;
	state->speed = 1.0;

	state->roadWidth = (RoadWidth) {.current = DEFAULT_ROAD_WIDTH, .next = DEFAULT_ROAD_WIDTH, .shouldUpdate = 0};
	state->direction = NONE;
	state->status = PLAYING;
}

void updateGameState(Application* app, GameState* state) {
	state->time += app->deltaTime;
	state->distance += state->speed * SPEED_MULTIPLIER * app->deltaTime;
	state->position += state->direction * SPEED_MULTIPLIER * app->deltaTime;
	state->score = (int)(state->distance / SCREEN_HEIGHT) * 50;
}