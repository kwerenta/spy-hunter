#include "application.h"

int initializeApplication(Application* app) {
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		printf("SDL_Init error: %s\n", SDL_GetError());
		return 0;
	}

	if (SDL_CreateWindowAndRenderer(SCREEN_WIDTH, SCREEN_HEIGHT, 0, &app->window, &app->renderer) != 0) {
		SDL_Quit();
		printf("SDL_CreateWindowAndRenderer error: %s\n", SDL_GetError());
		return 0;
	};

	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
	SDL_RenderSetLogicalSize(app->renderer, SCREEN_WIDTH, SCREEN_HEIGHT);
	SDL_SetRenderDrawColor(app->renderer, 0, 0, 0, 255);

	SDL_SetWindowTitle(app->window, "Spy Hunter");

	app->screen = SDL_CreateRGBSurface(0, SCREEN_WIDTH, SCREEN_HEIGHT, 32, 0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);
	app->screenTexture = SDL_CreateTexture(app->renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, SCREEN_WIDTH, SCREEN_HEIGHT);

	SDL_ShowCursor(SDL_DISABLE);

	app->saves = (Saves){ .count = 0, .list = NULL };

	return 1;
}

void loadBMP(SDL_Surface* surfaces[SURFACES_COUNT], Surfaces name, char* path) {
	surfaces[name] = SDL_LoadBMP(path);

	if (surfaces[name] == NULL)
		printf("SDL_LoadBMP(%s) error: %s\n", path, SDL_GetError());
}

int initializeSurfaces(Application* app) {
	loadBMP(app->surfaces, CHARSET_s, "./cs8x8.bmp");
	loadBMP(app->surfaces, CAR_s, "./car.bmp");
	loadBMP(app->surfaces, GRASS_s, "./bg.bmp");

	// Check if all surfaces loaded correctly
	for (int i = 0; i < SURFACES_COUNT; i++)
		if (app->surfaces[i] == NULL) {
			closeApplication(app);
			return 0;
		}

	SDL_SetColorKey(app->surfaces[CHARSET_s], 1, 0x000000);

	return 1;
}

void freeAllSurfaces(SDL_Surface* surfaces[SURFACES_COUNT]) {
	for (int i = 0; i < SURFACES_COUNT; i++)
		if (surfaces[i] != NULL)
			SDL_FreeSurface(surfaces[i]);
}

void closeApplication(Application* app) {
	freeAllSurfaces(app->surfaces);
	SDL_FreeSurface(app->screen);
	SDL_DestroyTexture(app->screenTexture);
	SDL_DestroyRenderer(app->renderer);
	SDL_DestroyWindow(app->window);

	SDL_Quit();
}

void updateScreen(Application* app) {
	SDL_UpdateTexture(app->screenTexture, NULL, app->screen->pixels, app->screen->pitch);
	SDL_RenderCopy(app->renderer, app->screenTexture, NULL, NULL);
	SDL_RenderPresent(app->renderer);
}

void createSaveList(Saves* saves) {
	FILE* saveListFile = fopen("./saves/list.bin", "rb");
	if (saves->list == NULL && saveListFile != NULL) {
		int prevSavesCount = saves->count;
		int saveCount = fread(saves, sizeof(Saves), 1, saveListFile);

		saves->count += prevSavesCount;
		saves->list = (SaveName*)malloc(saves->count * sizeof(SaveName));
		if (saves->list != NULL) {
			saveCount += fread(saves->list, sizeof(SaveName), saves->count, saveListFile);
			return;
		}
	}

	SaveName* newPtr = (SaveName*)realloc(saves->list, saves->count * sizeof(SaveName));
	saves->list = newPtr;
}