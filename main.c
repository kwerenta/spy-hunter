#define _USE_MATH_DEFINES
#include<math.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include "draw.h"
#include "application.h"
#include "game.h"
#include "gui.h"

#include"./SDL2-2.0.10/include/SDL.h"
#include"./SDL2-2.0.10/include/SDL_main.h"

void gameLoop(Application* app, GameState* state, SDL_Event* event, int *selection, int *prevTick, int *currTick, char* buffer);
void updateGameFrame(Application* app, GameState* state, char* buffer);

int main(int argc, char** argv) {
	Application app;
	GameState state;

	if (initializeApplication(&app) == 0) return 1;
	initializeGameState(&state);

	SDL_Event event;
	char buffer[128];
	int selection = 0, currTick, prevTick = SDL_GetTicks();

	while (state.status != QUIT) {
		gameLoop(&app, &state, &event, &selection, &prevTick, &currTick, buffer);
	};

	closeApplication(&app);
	return 0;
};

void gameLoop(Application* app, GameState* state, SDL_Event* event, int* selection, int* prevTick, int* currTick, char* buffer) {
	*currTick = SDL_GetTicks();
	app->deltaTime = (*currTick - *prevTick) * 0.001;
	*prevTick = *currTick;

	switch (state->status) {
	case PLAYING: updateGameFrame(app, state, buffer); break;
	case GAMEOVER: renderGameOver(app, state, buffer, *selection); break;
	case PAUSED: renderPause(app, buffer); break;
	case SAVE_SELECTION: renderGameSaveSelection(app, buffer, *selection); break;
	case SCOREBOARD: renderScoreboard(app, state, buffer); break;
	case SAVE_ERROR: renderConfirmation(app, buffer, SAVE_ERROR);  break;
	case LOAD_ERROR: renderConfirmation(app, buffer, LOAD_ERROR); break;
	case SAVE_SUCCESS: renderConfirmation(app, buffer, SAVE_SUCCESS); break;
	case LOAD_SUCCESS: renderConfirmation(app, buffer, LOAD_SUCCESS); break;
	default: break;
	}

	updateScreen(app);

	while (SDL_PollEvent(event)) {
		if (event->type == SDL_QUIT) state->status = QUIT;

		if (handleControls(state, event)) continue;
		switch (state->status) {
		case PLAYING: handleGameplay(state, event, &app->saves); break;
		case SAVE_SELECTION: handleSaveSelection(state, event, &app->saves, selection); break;
		case GAMEOVER: handleGameOver(state, event, &app->scoreboard, selection); break;
		case SCOREBOARD: handleScoreboard(state, event, &app->scoreboard); break;
		case SAVE_ERROR: case LOAD_ERROR: handleConfirmation(state, event, 1); break;
		case SAVE_SUCCESS: case LOAD_SUCCESS: handleConfirmation(state, event, 0); break;
		default: break;
		}
	};
}

void updateGameFrame(Application* app, GameState* state, char* buffer) {
	updateGameState(app, state);

	updateAI(app, state);

	renderBackground(app, state->backgroundOffset);

	updateRoadWidth(state);
	renderRoad(app, state);
	handleOutOfRoad(state, app->surfaces[CAR_s]);

	DrawSurface(app->screen, app->surfaces[CAR_s], SCREEN_WIDTH / 2 + state->position, CAR_Y_POSITION);
	renderAI(app, state);
	renderLegend(app, state, buffer);
	renderFunctionalities(app, buffer);
}