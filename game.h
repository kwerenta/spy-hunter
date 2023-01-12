#pragma once

#include <time.h>
#include <string.h>

#include "application.h"

#define SCORE_MULTIPLIER 3.5
#define SPEED_MULTIPLIER 400
#define DEFAULT_ROAD_WIDTH 200
#define AI_CARS_COUNT 4

#define IMMORTALITY_TIME 99
#define CAR_MILESTONE_1 7000
#define CAR_MILESTONE_2 10000
#define CAR_MILESTONE_LAST 20000

typedef enum {
	LEFT = -1,
	NONE = 0,
	RIGHT = 1
} H_Direction;

typedef enum GameStatus {
	PLAYING,
	PAUSED,
	SAVE_SELECTION,
	SCOREBOARD,
	GAMEOVER,
	QUIT
} GameStatus;

typedef struct RoadWidth {
	int current;
	int next;
	int lastUpdate;
} RoadWidth;

typedef struct SpareCars {
	int count;
	double lastMilestone;
} SpareCars;

typedef struct Position {
	double x;
	double y;
} Position;

typedef enum AICar_Type {
	ENEMY,
	NON_ENEMY
} AICar_Type;

typedef struct AICar {
	int hp;
	double speed;
	Position position;
	AICar_Type type;
} AICar;

typedef struct GameState {
	int score;
	int haltScore;
	double immortalityTime;
	double position;
	double time;
	double distance;
	double speed;
	SpareCars spareCars;
	AICar aiCars[AI_CARS_COUNT];
	RoadWidth roadWidth;
	H_Direction direction;
	GameStatus status;
} GameState;

void initializeGameState(GameState* state);
void updateGameState(Application* app, GameState* state);
void updateRoadWidth(GameState* state);
void updateAI(Application* app, GameState* state, int backgroundOffset);
void handleOutOfRoad(GameState* state, int backgroundOffset);
void handleControls(GameState* state, SDL_Event* event, Saves* saves);
void handleMovement(GameState* state, SDL_Event* event);
void handleSaveSelection(GameState* state, SDL_Event* event, Saves* saves, int* selection);
void handleGameOver(GameState* state, SDL_Event* event, Scoreboard* scoreboard, int* selection);
void handleScoreboard(GameState* state, SDL_Event* event, Scoreboard* scoreboard);
int loadGame(GameState* state, SaveName save);
int saveGame(GameState* state, Saves* saves);