#pragma once

#include <stdio.h>
#include <stdlib.h>

#include"./SDL2-2.0.10/include/SDL.h"
#include"./SDL2-2.0.10/include/SDL_main.h"

#define SCREEN_WIDTH	640
#define SCREEN_HEIGHT	480

#define SURFACES_COUNT 3

#define CAR_Y_POSITION SCREEN_HEIGHT * 3 / 4

#define DATETIME_LENGTH 18

typedef enum Surfaces {
	CHARSET_s,
	CAR_s,
	GRASS_s
} Surfaces;

typedef char SaveName[DATETIME_LENGTH];

typedef struct Saves {
	int count;
	SaveName* list;
} Saves;

typedef struct Result {
	double time;
	int score;
} Result;

typedef struct Scoreboard {
	int count;
	Result* list;
} Scoreboard;

typedef struct Application {
	SDL_Renderer* renderer;
	SDL_Window* window;
	SDL_Surface* screen;
	SDL_Texture* screenTexture;
	SDL_Surface* surfaces[SURFACES_COUNT];
	double deltaTime;
	Saves saves;
	Scoreboard scoreboard;
} Application;

int initializeApplication(Application* app);
void closeApplication(Application* app);
int initializeSurfaces(Application* app);
void updateScreen(Application* app);
void createSaveList(Saves* saves);
void createScoreboard(Scoreboard* scoreboard);
void saveScoreboard(Scoreboard* scoreboard);
void sortScoreboard(Scoreboard* scoreboard, int sortBy);