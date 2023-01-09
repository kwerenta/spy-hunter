#define _USE_MATH_DEFINES
#include<math.h>
#include<stdio.h>
#include<string.h>

#include "draw.h"
#include "application.h"
#include "game.h"
#include "gui.h"

#include"./SDL2-2.0.10/include/SDL.h"
#include"./SDL2-2.0.10/include/SDL_main.h"

#define SPEED_MULTIPLIER 200

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

	int prevTick, currTick, backgroundOffset, direction = 0;

	prevTick = SDL_GetTicks();

	while (state.status != QUIT) {
		currTick = SDL_GetTicks();

		app.deltaTime = (currTick - prevTick) * 0.001;
		prevTick = currTick;

		if (state.status == PLAYING) {
			state.time += app.deltaTime;
			state.distance += state.speed * SPEED_MULTIPLIER * app.deltaTime;
			state.position += direction * SPEED_MULTIPLIER * app.deltaTime;
			state.score = (int)(state.distance / SCREEN_HEIGHT) * 50;
		}

		backgroundOffset = (int)state.distance % SCREEN_HEIGHT;
		DrawSurface(app.screen, app.surfaces[GRASS_s], SCREEN_WIDTH / 2, backgroundOffset - SCREEN_HEIGHT / 2);
		DrawSurface(app.screen, app.surfaces[GRASS_s], SCREEN_WIDTH / 2, backgroundOffset + SCREEN_HEIGHT / 2);

		DrawRectangle(app.screen, SCREEN_WIDTH / 2 - state.roadWidth / 2, 0, state.roadWidth, SCREEN_HEIGHT, black, black);

		// Car sprite is 24px wide
		DrawSurface(app.screen, app.surfaces[CAR_s], SCREEN_WIDTH / 2 + state.position, SCREEN_HEIGHT * 2 / 3);

		renderLegend(&app, &state, buffer, blue, red);

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
				default:break;
				}

				if (state.status == PLAYING)
				{
					switch (event.key.keysym.sym)
					{
					case SDLK_UP: state.speed = 2.0; break;
					case SDLK_DOWN: state.speed = 0.3; break;
					case SDLK_RIGHT: direction = 1; break;
					case SDLK_LEFT: direction = -1; break;
					default: break;
					}
				}
				break;
			case SDL_KEYUP:
				state.speed = 1.0;
				if ((event.key.keysym.sym == SDLK_RIGHT && direction == 1) ||
					(event.key.keysym.sym == SDLK_LEFT && direction == -1))
					direction = 0;
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
