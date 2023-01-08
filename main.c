#define _USE_MATH_DEFINES
#include<math.h>
#include<stdio.h>
#include<string.h>

#include "draw.h"
#include "application.h"
#include "game.h"

#include"./SDL2-2.0.10/include/SDL.h"
#include"./SDL2-2.0.10/include/SDL_main.h"

#define FPS_REFRESH_RATE 2
#define FPS_REFRESH_TIME ((double)1 / FPS_REFRESH_RATE)

int main(int argc, char** argv) {
	Application app;
	SDL_Event event;

	GameState state;

	int prevTick, currTick, frames = 0, quit = 0;
	double fpsTimer = 0, fps = 0;

	if (initializeApplication(&app) != 0)
		return 1;

	loadBMP(&app, CHARSET_s, "./cs8x8.bmp");
	loadBMP(&app, ETI_s, "./eti.bmp");

	// Check if all surfaces loaded correctly
	int i;
	for (i = 0; i < SURFACES_COUNT; i++) {
		if (app.surfaces[i] == NULL) return 1;
	}

	SDL_SetColorKey(app.surfaces[CHARSET_s], 1, 0x000000);

	char text[128];
	int black = SDL_MapRGB(app.screen->format, 0x00, 0x00, 0x00);
	int green = SDL_MapRGB(app.screen->format, 0x00, 0xFF, 0x00);
	int red = SDL_MapRGB(app.screen->format, 0xFF, 0x00, 0x00);
	int blue = SDL_MapRGB(app.screen->format, 0x11, 0x11, 0xCC);

	initializeGameState(&state);

	prevTick = SDL_GetTicks();

	while (!quit) {
		currTick = SDL_GetTicks();

		app.deltaTime = (currTick - prevTick) * 0.001;
		prevTick = currTick;

		state.time += app.deltaTime;

		state.distance += state.speed * app.deltaTime;

		SDL_FillRect(app.screen, NULL, black);

		DrawSurface(app.screen, app.surfaces[ETI_s], SCREEN_WIDTH / 2 + sin(state.distance) * SCREEN_HEIGHT / 3, SCREEN_HEIGHT / 2 + cos(state.distance) * SCREEN_HEIGHT / 3);

		fpsTimer += app.deltaTime;
		if (fpsTimer > FPS_REFRESH_TIME) {
			fps = frames * FPS_REFRESH_RATE;
			frames = 0;
			fpsTimer -= FPS_REFRESH_TIME;
		};

		// info text
		DrawRectangle(app.screen, 4, 4, SCREEN_WIDTH - 8, 36, red, blue);
		sprintf_s(text, 128, "TIME %03.0lf  %.0lf frames / s", state.time, fps);
		DrawString(app.screen, 8, 10, text, app.surfaces[CHARSET_s]);

		sprintf_s(text, 128, "SCORE %05d", state.score);
		DrawString(app.screen, app.screen->w - 8 - strlen(text) * 8, 10, text, app.surfaces[CHARSET_s]);

		sprintf_s(text, 128, "Kamil Wenta 193437");
		DrawString(app.screen, app.screen->w / 2 - strlen(text) * 8 / 2, 26, text, app.surfaces[CHARSET_s]);

		SDL_UpdateTexture(app.screenTexture, NULL, app.screen->pixels, app.screen->pitch);
		SDL_RenderCopy(app.renderer, app.screenTexture, NULL, NULL);
		SDL_RenderPresent(app.renderer);

		while (SDL_PollEvent(&event)) {
			switch (event.type) {
			case SDL_KEYDOWN:
				if (event.key.keysym.sym == SDLK_ESCAPE) quit = 1;
				else if (event.key.keysym.sym == SDLK_UP) state.speed = 2.0;
				else if (event.key.keysym.sym == SDLK_DOWN) state.speed = 0.3;
				break;
			case SDL_KEYUP:
				state.speed = 1.0;
				break;
			case SDL_QUIT:
				quit = 1;
				break;
			};
		};
		frames++;
	};

	closeApplication(&app);
	return 0;
};
