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
	SDL_Event event;

	GameState state;

	if (initializeApplication(&app) != 0)
		return 1;

	loadBMP(&app, CHARSET_s, "./cs8x8.bmp");
	loadBMP(&app, CAR_s, "./car.bmp");
	loadBMP(&app, GRASS_s, "./bg.bmp");

	// Check if all surfaces loaded correctly
	for (int i = 0; i < SURFACES_COUNT; i++)
		if (app.surfaces[i] == NULL) return 1;

	SDL_SetColorKey(app.surfaces[CHARSET_s], 1, 0x000000);

	char buffer[128];
	int black = SDL_MapRGB(app.screen->format, 0x00, 0x00, 0x00);
	int grassGreen = SDL_MapRGB(app.screen->format, 0x37, 0xAE, 0x0F);
	int red = SDL_MapRGB(app.screen->format, 0xFF, 0x00, 0x00);
	int blue = SDL_MapRGB(app.screen->format, 0x11, 0x11, 0xCC);

	initializeGameState(&state);

	int currTick, backgroundOffset, prevTick = SDL_GetTicks();

	while (state.status != QUIT) {
		currTick = SDL_GetTicks();
		app.deltaTime = (currTick - prevTick) * 0.001;
		prevTick = currTick;

		if (state.status == PLAYING) {
			updateGameState(&app, &state);

			backgroundOffset = (int)state.distance % SCREEN_HEIGHT;
			DrawSurface(app.screen, app.surfaces[GRASS_s], SCREEN_WIDTH / 2, backgroundOffset - SCREEN_HEIGHT / 2);
			DrawSurface(app.screen, app.surfaces[GRASS_s], SCREEN_WIDTH / 2, backgroundOffset + SCREEN_HEIGHT / 2);

			if (backgroundOffset != 0) {
				state.roadWidth.shouldUpdate = 1;
			}
			else if (state.roadWidth.shouldUpdate == 1) {
				state.roadWidth.shouldUpdate = 0;
				state.roadWidth.current = state.roadWidth.next;
				state.roadWidth.next = DEFAULT_ROAD_WIDTH + (rand() % 3 - 1) * rand() % 5 * 20;
			}

			if ((abs(state.position) > state.roadWidth.current / 2 && backgroundOffset < CAR_Y_POSITION) ||
				(abs(state.position) > state.roadWidth.next / 2 && backgroundOffset >= CAR_Y_POSITION))
				state.position = 0;

			DrawRectangle(app.screen, SCREEN_WIDTH / 2 - state.roadWidth.current / 2, backgroundOffset, state.roadWidth.current, SCREEN_HEIGHT - backgroundOffset, black, black);
			DrawRectangle(app.screen, SCREEN_WIDTH / 2 - state.roadWidth.next / 2, 0, state.roadWidth.next, backgroundOffset, black, black);

			DrawSurface(app.screen, app.surfaces[CAR_s], SCREEN_WIDTH / 2 + state.position, CAR_Y_POSITION);

			renderLegend(&app, &state, buffer, blue, red);
			renderFunctionalities(&app, buffer, blue, red);
		}
		else if (state.status == PAUSED) {
			renderPause(&app, buffer);
		}

		SDL_UpdateTexture(app.screenTexture, NULL, app.screen->pixels, app.screen->pitch);
		SDL_RenderCopy(app.renderer, app.screenTexture, NULL, NULL);
		SDL_RenderPresent(app.renderer);

		while (SDL_PollEvent(&event)) {
			switch (event.type) {
			case SDL_KEYDOWN:
				switch (event.key.keysym.sym)
				{
				case SDLK_ESCAPE: state.status = QUIT; break;
				case SDLK_p: state.status = state.status == PAUSED ? PLAYING : PAUSED; break;
				case SDLK_n: initializeGameState(&state);
				default: break;
				}

				if (state.status == PLAYING)
				{
					switch (event.key.keysym.sym)
					{
					case SDLK_UP: state.speed = 2.0; break;
					case SDLK_DOWN: state.speed = 0.3; break;
					case SDLK_RIGHT: state.direction = RIGHT; break;
					case SDLK_LEFT: state.direction = LEFT; break;
					default: break;
					}
				}
				break;
			case SDL_KEYUP:
				state.speed = 1.0;
				if ((event.key.keysym.sym == SDLK_RIGHT && state.direction == RIGHT) ||
					(event.key.keysym.sym == SDLK_LEFT && state.direction == LEFT))
					state.direction = NONE;
				break;
			case SDL_QUIT:
				state.status = QUIT;
				break;
			};
		};
	};

	closeApplication(&app);
	return 0;
};
