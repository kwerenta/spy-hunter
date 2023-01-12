#include "game.h"

void initializeGameState(GameState* state) {
	state->score = 0;
	state->haltScore = 0;

	state->immortalityTime = IMMORTALITY_TIME;
	state->time = 0;
	state->position = 0;
	state->distance = 0;
	state->speed = 1.0;

	for (int i = 0; i < AI_CARS_COUNT; i++)
		state->aiCars[i].hp = 0;

	state->spareCars = (SpareCars){ .count = 0, .lastMilestone = 0 };
	state->roadWidth = (RoadWidth){ .current = DEFAULT_ROAD_WIDTH, .next = DEFAULT_ROAD_WIDTH, .lastUpdate = 0 };
	state->direction = NONE;
	state->status = PLAYING;
}

void updateSpareCars(SpareCars* spareCars, int score) {
	if (spareCars->lastMilestone == 0 && score >= CAR_MILESTONE_1) {
		spareCars->lastMilestone = CAR_MILESTONE_1;
		spareCars->count++;
	}
	else if (spareCars->lastMilestone == CAR_MILESTONE_1 && score >= CAR_MILESTONE_2) {
		spareCars->lastMilestone = CAR_MILESTONE_2;
		spareCars->count++;
	}
	else {
		int currentMilestone = (int)(score / CAR_MILESTONE_LAST) * CAR_MILESTONE_LAST;
		if (currentMilestone > spareCars->lastMilestone) {
			spareCars->lastMilestone = currentMilestone;
			spareCars->count++;
		}
	}
}

void updateImmortalityTime(double* immortalityTime, double deltaTime) {
	if (*immortalityTime > 0)
		*immortalityTime -= deltaTime;
	else
		*immortalityTime = 0;
}

void updateGameState(Application* app, GameState* state) {
	state->time += app->deltaTime;
	state->distance += state->speed * SPEED_MULTIPLIER * app->deltaTime;
	state->position += state->direction * SPEED_MULTIPLIER * app->deltaTime;
	state->score = (int)(state->distance / SCREEN_HEIGHT * SCORE_MULTIPLIER) * 50;

	updateSpareCars(&state->spareCars, state->score);
	updateImmortalityTime(&state->immortalityTime, app->deltaTime);
}

void updateRoadWidth(GameState* state) {
	int fullscreenDistance = (int)(state->distance / SCREEN_HEIGHT);
	if (state->roadWidth.lastUpdate != fullscreenDistance) {
		state->roadWidth.lastUpdate = fullscreenDistance;
		state->roadWidth.current = state->roadWidth.next;
		state->roadWidth.next = DEFAULT_ROAD_WIDTH + (rand() % 3 - 1) * rand() % 5 * 20;
	}
}

void createAICar(AICar *aiCar) {
	aiCar->type = rand() % 2 ? ENEMY : NON_ENEMY;
	aiCar->hp = aiCar->type == NON_ENEMY ? 3 : 5;
	aiCar->speed = 1.0 + (rand() % 2 - 1) * rand() % 3 / 10.0;
	aiCar->position.x = rand() % 5 * (rand() % 2 + 1) * 10;
	aiCar->position.y = SCREEN_HEIGHT / 3 + rand() % 5 * 10;
}

void updateAI(Application* app, GameState* state, int backgroundOffset) {
	int fullscreenDistance = (int)(state->distance / SCREEN_HEIGHT);
	int firstFreeIndex = 0;

	for (int i = 0; i < AI_CARS_COUNT; i++)
	{
		AICar* aiCar = &state->aiCars[i];
		if (aiCar->hp == 0) continue;
		aiCar->position.y -= app->deltaTime * (aiCar->speed - state->speed) * SPEED_MULTIPLIER;

		// Check if out of screen (top or bottom)
		int surfaceHeight = app->surfaces[aiCar->type == NON_ENEMY ? NON_ENEMY_CAR_s : ENEMY_CAR_s]->h;
		if (aiCar->position.y > SCREEN_HEIGHT + surfaceHeight ||
			aiCar->position.y < - surfaceHeight)
		{
			aiCar->hp = 0;
			continue;
		}

		if (i == firstFreeIndex) firstFreeIndex++;

		if(state->roadWidth.lastUpdate != fullscreenDistance)
			aiCar->position.x = rand() % 5 * 10;
	}

	if (state->roadWidth.lastUpdate != fullscreenDistance && firstFreeIndex < AI_CARS_COUNT && rand() % 3 == 0) {
		createAICar(&state->aiCars[firstFreeIndex]);
	}
}

void handleOutOfRoad(GameState* state, int backgroundOffset) {
	if (abs(state->position) > (backgroundOffset >= CAR_Y_POSITION ? state->roadWidth.next : state->roadWidth.current) / 2)
	{
		state->position = 0;
		if (state->immortalityTime > 0) return;

		if (state->spareCars.count == 0) {
			state->status = GAMEOVER;
			return;
		}

		state->spareCars.count--;
	}
}

void handleControls(GameState* state, SDL_Event* event, Saves* saves) {
	if (event->type == SDL_KEYDOWN) {
		switch (event->key.keysym.sym) {
		case SDLK_p: state->status = state->status == PAUSED ? PLAYING : PAUSED; break;
		case SDLK_ESCAPE: state->status = QUIT; break;
		case SDLK_n: initializeGameState(state); break;
		case SDLK_s: state->status = saveGame(state, saves) == 1 ? PAUSED : QUIT; break;
		case SDLK_l: createSaveList(saves); state->status = SAVE_SELECTION; break;
		default: break;
		}
	}
};

void handleGameplay(GameState* state, SDL_Event* event) {
	switch (event->type) {
	case SDL_KEYDOWN:
		switch (event->key.keysym.sym) {
		case SDLK_f: state->status = GAMEOVER; break;
		case SDLK_UP: state->speed = 1.75; break;
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

void handleGameOver(GameState* state, SDL_Event* event, Scoreboard* scoreboard, int* selection) {
	if (event->type != SDL_KEYDOWN) return;

	switch (event->key.keysym.sym) {
	case SDLK_LEFT: case SDLK_RIGHT: *selection = *selection == 0 ? 1 : 0; break;
	case SDLK_RETURN:
		if (*selection == 0) {
			scoreboard->count++;
			createScoreboard(scoreboard);
			scoreboard->list[scoreboard->count - 1] = (Result){ .score = state->score, .time = state->time };
			saveScoreboard(scoreboard);
		}
		else {
			createScoreboard(scoreboard);
		}
		state->status = SCOREBOARD;
		*selection = 0;
		break;
	default: break;
	}
}

void handleScoreboard(GameState* state, SDL_Event* event, Scoreboard* scoreboard) {
	if (event->type != SDL_KEYDOWN) return;

	switch (event->key.keysym.sym) {
	case SDLK_p: sortScoreboard(scoreboard, 0); break;
	case SDLK_t: sortScoreboard(scoreboard, 1); break;
	case SDLK_RETURN: initializeGameState(state); break;
	default: break;
	}
}

void getGameSavePath(char buffer[DATETIME_LENGTH + 12], SaveName save) {
	sprintf_s(buffer, DATETIME_LENGTH + 12, "%s%s%s", "./saves/", save, ".bin");
}

int saveGame(GameState* state, Saves* saves) {
	char datetime[DATETIME_LENGTH], buffer[DATETIME_LENGTH + 12];
	time_t timestamp = time(NULL);
	struct tm* time = localtime(&timestamp);

	strftime(datetime, sizeof(datetime), "%F_%H%M%S", time);
	getGameSavePath(buffer, datetime);
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
	getGameSavePath(buffer, save);
	FILE* saveFile = fopen(buffer, "rb");

	if (saveFile == NULL) return 0;

	int loadCount = fread(state, sizeof(GameState), 1, saveFile);
	fclose(saveFile);

	return loadCount;
}