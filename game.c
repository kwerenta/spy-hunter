#include "game.h"

void initializeGameState(GameState* state) {
	state->score = 0;
	state->haltScore = 0;
	state->carsLeft = 3;

	state->position = 0;
	state->time = 0;
	state->distance = 0;
	state->speed = 1.0;

	state->roadWidth = (RoadWidth){ .current = DEFAULT_ROAD_WIDTH, .next = DEFAULT_ROAD_WIDTH, .shouldUpdate = 0 };
	state->direction = NONE;
	state->status = PLAYING;
}

void updateGameState(Application* app, GameState* state) {
	state->time += app->deltaTime;
	state->distance += state->speed * SPEED_MULTIPLIER * app->deltaTime;
	state->position += state->direction * SPEED_MULTIPLIER * app->deltaTime;
	state->score = (int)(state->distance / SCREEN_HEIGHT) * 50;
}

void updateRoadWidth(GameState* state, int backgroundOffset) {
	if (backgroundOffset != 0) {
		state->roadWidth.shouldUpdate = 1;
		return;
	}

	if (state->roadWidth.shouldUpdate == 1) {
		state->roadWidth.shouldUpdate = 0;
		state->roadWidth.current = state->roadWidth.next;
		state->roadWidth.next = DEFAULT_ROAD_WIDTH + (rand() % 3 - 1) * rand() % 5 * 20;
	}
}

void handleControls(GameState* state, SDL_Event* event) {
	if (event->type == SDL_KEYDOWN) {
		switch (event->key.keysym.sym) {
		case SDLK_ESCAPE: state->status = QUIT; break;
		case SDLK_p: state->status = state->status == PAUSED ? PLAYING : PAUSED; break;
		case SDLK_n: initializeGameState(state); break;
		case SDLK_s: state->status = saveGame(state) == 1 ? PAUSED : QUIT; break;
		case SDLK_l: state->status = loadGame(state) == 1 ? PAUSED : QUIT; break;
		default: break;
		}
	}
};

void handleMovement(GameState* state, SDL_Event* event) {
	switch (event->type) {
	case SDL_KEYDOWN:
		switch (event->key.keysym.sym) {
		case SDLK_UP: state->speed = 2.0; break;
		case SDLK_DOWN: state->speed = 0.3; break;
		case SDLK_RIGHT: state->direction = RIGHT; break;
		case SDLK_LEFT: state->direction = LEFT; break;
		default: break;
		}
		break;
	case SDL_KEYUP:
		if (event->key.keysym.sym == SDLK_UP || event->key.keysym.sym == SDLK_DOWN)
			state->speed = 1.0;
		else if ((event->key.keysym.sym == SDLK_RIGHT && state->direction == RIGHT) ||
			(event->key.keysym.sym == SDLK_LEFT && state->direction == LEFT))
			state->direction = NONE;
		break;
	};
};

int saveGame(GameState* state) {
	char datetime[18], buffer[30];
	time_t timestamp = time(NULL);
	struct tm* time = localtime(&timestamp);

	strftime(datetime, sizeof(datetime), "%F_%H%M%S", time);
	sprintf_s(buffer, sizeof(buffer), "%s%s%s", "./saves/", datetime, ".bin");
	FILE* saveFile = fopen(buffer, "wb");

	if (saveFile == NULL) return 0;

	int saveCount = fwrite(state, sizeof(GameState), 1, saveFile);
	fclose(saveFile);

	return saveCount;
}

int loadGame(GameState* state) {
	// TEMP Fixed saveFile name
	FILE* saveFile = fopen("./saves/2023-01-10_202529.bin", "rb");

	if (saveFile == NULL) return 0;

	int loadCount = fread(state, sizeof(GameState), 1, saveFile);
	fclose(saveFile);

	return loadCount;
}