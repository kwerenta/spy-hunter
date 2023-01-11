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

int main(int argc, char** argv) {
	Application app;
	GameState state;

	if (initializeApplication(&app) == 0)
		return 1;

	if (initializeSurfaces(&app) == 0)
		return 1;

	initializeGameState(&state);

	SDL_Event event;

	char buffer[128];
	int black = SDL_MapRGB(app.screen->format, 0x00, 0x00, 0x00);
	int red = SDL_MapRGB(app.screen->format, 0xFF, 0x00, 0x00);
	int blue = SDL_MapRGB(app.screen->format, 0x11, 0x11, 0xCC);

	int selection = 0, backgroundOffset, currTick, prevTick = SDL_GetTicks();

	while (state.status != QUIT) {
		currTick = SDL_GetTicks();
		app.deltaTime = (currTick - prevTick) * 0.001;
		prevTick = currTick;

		if (state.status == PLAYING) {
			updateGameState(&app, &state);

			backgroundOffset = (int)state.distance % SCREEN_HEIGHT;
			renderBackground(&app, backgroundOffset);

			updateRoadWidth(&state);
			renderRoad(&app, &state, backgroundOffset, black);
			handleOutOfRoad(&state, backgroundOffset);

			DrawSurface(app.screen, app.surfaces[CAR_s], SCREEN_WIDTH / 2 + state.position, CAR_Y_POSITION);

			renderLegend(&app, &state, buffer, blue, red);
			renderFunctionalities(&app, buffer, blue, red);
		}
		else if (state.status == GAMEOVER) 
			renderGameOver(&app, &state, buffer, blue, red, black, selection);
		else if (state.status == PAUSED) 
			renderPause(&app, buffer);
		else if (state.status == SAVE_SELECTION)
			renderGameSaveSelection(&app, buffer, blue, red, black, selection);
		else if (state.status == SCOREBOARD)
			renderScoreboard(&app, &state, buffer, blue, red);

		updateScreen(&app);

		while (SDL_PollEvent(&event)) {
			handleControls(&state, &event, &(app.saves));
			if (event.type == SDL_QUIT)
				state.status = QUIT;
			else if (state.status == PLAYING)
				handleGameplay(&state, &event);
			else if (state.status == SAVE_SELECTION)
				handleSaveSelection(&state, &event, &app.saves, &selection);
			else if (state.status == GAMEOVER)
				handleGameOver(&state, &event, &app.scoreboard, &selection);
			else if (state.status == SCOREBOARD)
				handleScoreboard(&state, &event, &app.scoreboard);
		};
	};

	closeApplication(&app);
	return 0;
};
