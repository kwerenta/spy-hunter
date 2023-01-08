#define _USE_MATH_DEFINES
#include<math.h>
#include<stdio.h>
#include<string.h>

#include "draw.h"
#include "application.h"

#include"./SDL2-2.0.10/include/SDL.h"
#include"./SDL2-2.0.10/include/SDL_main.h"

#define FPS_REFRESH_RATE 2
#define FPS_REFRESH_TIME ((double)1 / FPS_REFRESH_RATE)

int main(int argc, char** argv) {
	Application app;
	SDL_Event event;

	int prevTick, currTick, quit, frames;
	double worldTime, fpsTimer, fps, distance, etiSpeed;

	if (initializeApplication(&app) != 0)
		return 1;

	app.surfaces[0] = SDL_LoadBMP("./cs8x8.bmp");
	if (app.surfaces[0] == NULL) {
		printf("SDL_LoadBMP(cs8x8.bmp) error: %s\n", SDL_GetError());
		SDL_FreeSurface(app.screen);
		SDL_DestroyTexture(app.screenTexture);
		SDL_DestroyWindow(app.window);
		SDL_DestroyRenderer(app.renderer);
		SDL_Quit();
		return 1;
	};
	SDL_SetColorKey(app.surfaces[0], 1, 0x000000);

	app.surfaces[1] = SDL_LoadBMP("./eti.bmp");
	if (app.surfaces[1] == NULL) {
		printf("SDL_LoadBMP(eti.bmp) error: %s\n", SDL_GetError());
		SDL_FreeSurface(app.surfaces[0]);
		SDL_FreeSurface(app.screen);
		SDL_DestroyTexture(app.screenTexture);
		SDL_DestroyWindow(app.window);
		SDL_DestroyRenderer(app.renderer);
		SDL_Quit();
		return 1;
	};

	char text[128];
	int black = SDL_MapRGB(app.screen->format, 0x00, 0x00, 0x00);
	int green = SDL_MapRGB(app.screen->format, 0x00, 0xFF, 0x00);
	int red = SDL_MapRGB(app.screen->format, 0xFF, 0x00, 0x00);
	int blue = SDL_MapRGB(app.screen->format, 0x11, 0x11, 0xCC);

	prevTick = SDL_GetTicks();

	frames = 0;
	fpsTimer = 0;
	fps = 0;
	quit = 0;
	worldTime = 0;
	distance = 0;
	etiSpeed = 1;

	while (!quit) {
		currTick = SDL_GetTicks();

		app.deltaTime = (currTick - prevTick) * 0.001;
		prevTick = currTick;

		worldTime += app.deltaTime;

		distance += etiSpeed * app.deltaTime;

		SDL_FillRect(app.screen, NULL, black);

		DrawSurface(app.screen, app.surfaces[1], SCREEN_WIDTH / 2 + sin(distance) * SCREEN_HEIGHT / 3, SCREEN_HEIGHT / 2 + cos(distance) * SCREEN_HEIGHT / 3);

		fpsTimer += app.deltaTime;
		if (fpsTimer > FPS_REFRESH_TIME) {
			fps = frames * FPS_REFRESH_RATE;
			frames = 0;
			fpsTimer -= FPS_REFRESH_TIME;
		};

		// info text
		DrawRectangle(app.screen, 4, 4, SCREEN_WIDTH - 8, 36, red, blue);
		sprintf_s(text, 128, "Template for the second project, elapsed time = %.1lf s  %.0lf frames / s", worldTime, fps);
		DrawString(app.screen, app.screen->w / 2 - strlen(text) * 8 / 2, 10, text, app.surfaces[0]);
		sprintf_s(text, 128, "Esc - exit, \030 - faster, \031 - slower");
		DrawString(app.screen, app.screen->w / 2 - strlen(text) * 8 / 2, 26, text, app.surfaces[0]);

		SDL_UpdateTexture(app.screenTexture, NULL, app.screen->pixels, app.screen->pitch);
		SDL_RenderCopy(app.renderer, app.screenTexture, NULL, NULL);
		SDL_RenderPresent(app.renderer);

		while (SDL_PollEvent(&event)) {
			switch (event.type) {
			case SDL_KEYDOWN:
				if (event.key.keysym.sym == SDLK_ESCAPE) quit = 1;
				else if (event.key.keysym.sym == SDLK_UP) etiSpeed = 2.0;
				else if (event.key.keysym.sym == SDLK_DOWN) etiSpeed = 0.3;
				break;
			case SDL_KEYUP:
				etiSpeed = 1.0;
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
