#pragma once

typedef enum {
	PLAYING,
	PAUSED,
	QUIT
} GameStatus;

typedef struct {
	int score;
	int haltScore;
	int carsLeft;
	int roadWidth;
	double position;
	double time;
	double distance;
	double speed;
	GameStatus status;
} GameState;

void initializeGameState(GameState* state);
