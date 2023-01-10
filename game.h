#pragma once

#include "application.h"

#define SPEED_MULTIPLIER 200
#define DEFAULT_ROAD_WIDTH 200

typedef enum {
	LEFT = -1,
	NONE = 0,
	RIGHT = 1
} H_Direction;

typedef enum {
	PLAYING,
	PAUSED,
	QUIT
} GameStatus;

typedef struct RoadWidth {
	int current;
	int next;
	int shouldUpdate;
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