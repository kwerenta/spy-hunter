#include "application.h"
#include <stdio.h>

int initializeApplication(Application* app) {
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		printf("SDL_Init error: %s\n", SDL_GetError());
		return 1;
	}

	if (SDL_CreateWindowAndRenderer(SCREEN_WIDTH, SCREEN_HEIGHT, 0, &app->window, &app->renderer) != 0) {
		SDL_Quit();
		printf("SDL_CreateWindowAndRenderer error: %s\n", SDL_GetError());
		return 1;
	};

	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
	SDL_RenderSetLogicalSize(app->renderer, SCREEN_WIDTH, SCREEN_HEIGHT);
	SDL_SetRenderDrawColor(app->renderer, 0, 0, 0, 255);

	SDL_SetWindowTitle(app->window, "Spy Hunter");

	app->screen = SDL_CreateRGBSurface(0, SCREEN_WIDTH, SCREEN_HEIGHT, 32, 0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);
	app->screenTexture = SDL_CreateTexture(app->renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, SCREEN_WIDTH, SCREEN_HEIGHT);

	SDL_ShowCursor(SDL_DISABLE);

	return 0;
}

void loadBMP(Application* app, Surfaces name, char* path) {
	app->surfaces[name] = SDL_LoadBMP(path);

	if (app->surfaces[name] == NULL) {
		printf("SDL_LoadBMP(%s) error: %s\n", path, SDL_GetError());
		closeApplication(app);
	};
}

void freeAllSurfaces(Application* app) {
	for (int i = 0; i < SURFACES_COUNT; i++)
	{
		if (app->surfaces[i] != NULL)
			SDL_FreeSurface(app->surfaces[i]);
	}
}

void closeApplication(Application* app) {
	freeAllSurfaces(app);
	SDL_FreeSurface(app->screen);
	SDL_DestroyTexture(app->screenTexture);
	SDL_DestroyRenderer(app->renderer);
	SDL_DestroyWindow(app->window);

	SDL_Quit();
}