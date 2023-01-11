#pragma once

#include <time.h>
#include <string.h>

#include "application.h"

#define SPEED_MULTIPLIER 200
#define DEFAULT_ROAD_WIDTH 200

typedef enum {
	LEFT = -1,
	NONE = 0,
	RIGHT = 1
} H_Direction;

typedef enum GameStatus {
	PLAYING,
	PAUSED,
	SAVE_SELECTION,
	QUIT
} GameStatus;

typedef struct RoadWidth {
	int current;
	int next;
	int lastUpdate;
} RoadWidth;

typedef struct GameState {
	int score;
	int haltScore;
	int carsLeft;
	double position;
	double time;
	double distance;
	double speed;
	RoadWidth roadWidth;
	H_Direction direction;
	GameStatus status;
} GameState;

void initializeGameState(GameState* state);
void updateGameState(Application* app, GameState* state);
void updateRoadWidth(GameState* state);
void handleControls(GameState* state, SDL_Event* event, Saves* saves);
void handleMovement(GameState* state, SDL_Event* event);
void handleSaveSelection(GameState* state, SDL_Event* event, Saves* saves, int* selection);
int loadGame(GameState* state, SaveName save);
int saveGame(GameState* state, Saves* saves);