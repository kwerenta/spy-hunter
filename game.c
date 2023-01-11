#include "game.h"

void initializeGameState(GameState* state) {
	state->score = 0;
	state->haltScore = 0;
	state->carsLeft = 3;

	state->position = 0;
	state->time = 0;
	state->distance = 0;
	state->speed = 1.0;

	state->roadWidth = (RoadWidth){ .current = DEFAULT_ROAD_WIDTH, .next = DEFAULT_ROAD_WIDTH, .lastUpdate = 0 };
	state->direction = NONE;
	state->status = PLAYING;
}

void updateGameState(Application* app, GameState* state) {
	state->time += app->deltaTime;
	state->distance += state->speed * SPEED_MULTIPLIER * app->deltaTime;
	state->position += state->direction * SPEED_MULTIPLIER * app->deltaTime;
	state->score = (int)(state->distance / SCREEN_HEIGHT) * 50;
}

void updateRoadWidth(GameState* state) {
	int fullscreenDistance = (int)(state->distance / SCREEN_HEIGHT);
	if (state->roadWidth.lastUpdate != fullscreenDistance) {
		state->roadWidth.lastUpdate = fullscreenDistance;
		state->roadWidth.current = state->roadWidth.next;
		state->roadWidth.next = DEFAULT_ROAD_WIDTH + (rand() % 3 - 1) * rand() % 5 * 20;
	}
}

void handleControls(GameState* state, SDL_Event* event, Saves* saves) {
	if (event->type == SDL_KEYDOWN) {
		switch (event->key.keysym.sym) {
		case SDLK_ESCAPE: state->status = QUIT; break;
		case SDLK_p: state->status = state->status == PAUSED ? PLAYING : PAUSED; break;
		case SDLK_n: initializeGameState(state); break;
		case SDLK_s: state->status = saveGame(state, saves) == 1 ? PAUSED : QUIT; break;
		case SDLK_l: createSaveList(saves); state->status = SAVE_SELECTION; break;
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

void handleSaveSelection(GameState* state, SDL_Event* event, Saves* saves, int* selection) {
	if (saves->list == NULL || saves->count == 0) {
		state->status = PLAYING;
		return;
	}

	if (event->type != SDL_KEYDOWN) return;

	switch (event->key.keysym.sym) {
	case SDLK_UP: (*selection)--; break;
	case SDLK_DOWN: (*selection)++; break;
	case SDLK_RETURN: 
		state->status = loadGame(state, saves->list[*selection]) == 1 ? PAUSED : QUIT; 
		*selection = 0;
		break;
	default: break;
	}

	*selection = (*selection + saves->count) % saves->count;
}

int saveGame(GameState* state, Saves* saves) {
	char datetime[DATETIME_LENGTH], buffer[DATETIME_LENGTH + 12];
	time_t timestamp = time(NULL);
	struct tm* time = localtime(&timestamp);

	strftime(datetime, sizeof(datetime), "%F_%H%M%S", time);
	sprintf_s(buffer, sizeof(buffer), "%s%s%s", "./saves/", datetime, ".bin");
	FILE* saveFile = fopen(buffer, "wb");
	if (saveFile == NULL) return 0;

	int saveCount = fwrite(state, sizeof(GameState), 1, saveFile);
	fclose(saveFile);

	saves->count++;
	createSaveList(saves);
	strcpy(saves->list[saves->count - 1], datetime);

	FILE* saveListFile = fopen("./saves/list.bin", "wb");
	if (saveListFile == NULL) return 0;

	saveCount += fwrite(saves, sizeof(Saves), 1, saveListFile);
	saveCount += fwrite(saves->list, sizeof(SaveName), saves->count, saveListFile);
	fclose(saveListFile);

	return saveCount == 2 + saves->count;
}

int loadGame(GameState* state, SaveName save) {
	char buffer[DATETIME_LENGTH + 12];
	sprintf_s(buffer, sizeof(buffer), "%s%s%s", "./saves/", save, ".bin");
	FILE* saveFile = fopen(buffer, "rb");

	if (saveFile == NULL) return 0;

	int loadCount = fread(state, sizeof(GameState), 1, saveFile);
	fclose(saveFile);

	return loadCount;
}