#pragma once

#include"./SDL2-2.0.10/include/SDL.h"
#include"./SDL2-2.0.10/include/SDL_main.h"

#define SCREEN_WIDTH	640
#define SCREEN_HEIGHT	480

#define SURFACES_COUNT 2

typedef enum {
	CHARSET_s,
	CAR_s
} Surfaces;

typedef struct {
	SDL_Renderer* renderer;
	SDL_Window* window;
	SDL_Surface* screen;
	SDL_Texture* screenTexture;
	SDL_Surface* surfaces[2];
	double deltaTime;
} Application;

int initializeApplication(Application* app);

void closeApplication(Application* app);

void loadBMP(Application* app, Surfaces name, char* path);