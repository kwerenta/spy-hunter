#include "game.h"

void initializeGameState(GameState* state) {
	state->score = 0;
	state->additionalScore = 0;
	state->backgroundOffset = 0;
	state->screenDistance = 0;

	state->immortalityTime = IMMORTALITY_TIME;
	state->time = 0;
	state->position = 0;
	state->distance = 0;
	state->speed = 0;

	for (int i = 0; i < AI_CARS_COUNT; i++)
		state->aiCars[i].hp = 0;

	state->haltedScore = (HaltedScore){ .distance = 0, .time = 0 };
	state->spareCars = (SpareCars){ .count = 0, .lastMilestone = 0 };
	state->roadWidth = (RoadWidth){ .current = DEFAULT_ROAD_WIDTH, .next = DEFAULT_ROAD_WIDTH, .lastUpdate = 0 };
	state->direction = (Direction){ .horizontal = STRAIGHT, .vertical = NONE };
	state->status = PLAYING;
}

void updateGameState(Application* app, GameState* state) {
	state->time += app->deltaTime;
	state->position += state->direction.horizontal * state->speed * SPEED_MULTIPLIER / 2 * app->deltaTime;

	double deltaDistance = state->speed * SPEED_MULTIPLIER * app->deltaTime;
	state->distance += deltaDistance;
	if (state->haltedScore.time > 0 ||
		abs(state->position) + app->surfaces[CAR_s]->w / 2 > (state->backgroundOffset >= CAR_Y_POSITION ? state->roadWidth.next : state->roadWidth.current) / 2)
		state->haltedScore.distance += deltaDistance;

	state->score = (int)((state->distance - state->haltedScore.distance) / SCREEN_HEIGHT * SCORE_MULTIPLIER) * 50 + state->additionalScore;

	state->speed += state->direction.vertical * ACCELERATION_MULTIPLIER * app->deltaTime;
	state->speed = state->speed > MAX_SPEED ? MAX_SPEED : state->speed < 0 ? 0 : state->speed;

	state->backgroundOffset = (int)state->distance % SCREEN_HEIGHT;
	state->screenDistance = (int)(state->distance / SCREEN_HEIGHT);

	updateSpareCars(&state->spareCars, state->score);
	updateTimer(&state->immortalityTime, app->deltaTime);
	updateTimer(&state->haltedScore.time, app->deltaTime);
}

void updateRoadWidth(GameState* state) {
	if (state->roadWidth.lastUpdate == state->screenDistance)  return;
	state->roadWidth.lastUpdate = state->screenDistance;
	state->roadWidth.current = state->roadWidth.next;
	state->roadWidth.next = DEFAULT_ROAD_WIDTH + (rand() % 3 - 1) * rand() % 5 * 20;

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
		if (currentMilestone <= spareCars->lastMilestone) return;
		spareCars->lastMilestone = currentMilestone;
		spareCars->count++;
	}
}

void updateTimer(double* time, double deltaTime) {
	if (*time > 0) *time -= deltaTime;
	else *time = 0;
}

int isOutOfRoad(GameState* state, SDL_Surface* surface, double x, double y) {
	return abs(x) - surface->w / 2 > (state->backgroundOffset >= y ? state->roadWidth.next : state->roadWidth.current) / 2;
}

void createAICar(AICar* aiCar) {
	aiCar->type = rand() % 2 ? ENEMY : NON_ENEMY;
	aiCar->hp = aiCar->type == NON_ENEMY ? NON_ENEMY_HP : ENEMY_HP;
	aiCar->speed = 1.0 + (rand() % 3 - 1) * rand() % 5 / 10.0;
	aiCar->position.y = SCREEN_HEIGHT / 3 + rand() % 3 * 10;
	aiCar->position.x = rand() % 5 * (rand() % 3 - 1) * 10;
	aiCar->targetX = aiCar->position.x;
}

void updateAI(Application* app, GameState* state) {
	int firstFreeIndex = 0;

	for (int i = 0; i < AI_CARS_COUNT; i++)
	{
		AICar* aiCar = &state->aiCars[i];
		if (aiCar->hp == 0) continue;
		aiCar->position.y -= app->deltaTime * (aiCar->speed - state->speed) * SPEED_MULTIPLIER;

		H_Direction direction = aiCar->targetX > aiCar->position.x ? RIGHT :
			aiCar->targetX < aiCar->position.x ? LEFT : STRAIGHT;
		aiCar->position.x += direction * aiCar->speed * SPEED_MULTIPLIER / 2 * app->deltaTime;
		if (aiCar->position.x < aiCar->targetX && direction == LEFT ||
			aiCar->position.x > aiCar->targetX && direction == RIGHT)
			aiCar->position.x = aiCar->targetX;

		handleCollisions(app, state, aiCar, direction);

		// Check if out of screen (top or bottom)
		int surfaceHeight = app->surfaces[aiCar->type == NON_ENEMY ? NON_ENEMY_CAR_s : ENEMY_CAR_s]->h;
		if (aiCar->position.y > SCREEN_HEIGHT + surfaceHeight ||
			aiCar->position.y < -surfaceHeight)
		{
			aiCar->hp = 0;
			continue;
		}

		// Check if out of road
		if (isOutOfRoad(state, app->surfaces[aiCar->type == NON_ENEMY ? NON_ENEMY_CAR_s : ENEMY_CAR_s], aiCar->position.x, aiCar->position.y)) {
			if (aiCar->type == ENEMY && aiCar->hp < ENEMY_HP)
				state->additionalScore += SCORE_PER_ENEMY;
			else if (aiCar->type == NON_ENEMY && aiCar->hp < NON_ENEMY)
				state->haltedScore.time = HALT_SCORE_TIME;
			aiCar->hp = 0;
			continue;
		}

		if (i == firstFreeIndex) firstFreeIndex++;

		if (state->roadWidth.lastUpdate != state->screenDistance && rand() % 3 == 0)
			aiCar->targetX = (rand() % 3 - 1) * rand() % 5 * 20;
	}

	if (state->roadWidth.lastUpdate != state->screenDistance && firstFreeIndex < AI_CARS_COUNT && rand() % 3 == 0) {
		createAICar(&state->aiCars[firstFreeIndex]);
	}
}

void handleCollisions(Application* app, GameState* state, AICar* aiCar, H_Direction aiDirection) {
	if (aiCar->hp == 0) return;

	SDL_Surface* surface = app->surfaces[aiCar->type == NON_ENEMY ? NON_ENEMY_CAR_s : ENEMY_CAR_s];

	// Check that the surfaces overlap
	if (aiCar->position.x + surface->w >= state->position
		&& aiCar->position.x - surface->w <= state->position
		&& aiCar->position.y <= CAR_Y_POSITION + surface->h
		&& aiCar->position.y >= CAR_Y_POSITION - surface->h)
	{
		if (state->direction.horizontal != STRAIGHT) {
			state->position -= 20 * state->direction.horizontal;
			aiCar->position.x += 30 * state->direction.horizontal;

			aiCar->hp--;
			if (aiCar->hp == 0) {
				if (aiCar->type == NON_ENEMY) state->haltedScore.time = HALT_SCORE_TIME;
				else state->additionalScore += SCORE_PER_ENEMY;
			}

			state->direction.horizontal = STRAIGHT;
			state->speed *= 0.90;
		}
		else if (aiDirection != STRAIGHT) {
			state->position -= 30 * aiDirection;
			aiCar->position.x += 20 * aiDirection;

			state->direction.horizontal = STRAIGHT;
			state->speed *= 0.5;
		}
		else {
			if (aiCar->position.y > CAR_Y_POSITION)
				aiCar->speed *= 0.25;
			else
				state->speed *= 0.25;
		}

		aiCar->targetX = aiCar->position.x;
	}
}

void handleOutOfRoad(GameState* state, SDL_Surface* surface) {
	if (!isOutOfRoad(state, surface, state->position, CAR_Y_POSITION)) return;
	state->position = 0;
	state->speed = 0;
	if (state->immortalityTime > 0) return;

	if (state->spareCars.count == 0) {
		state->status = GAMEOVER;
		return;
	}

	state->spareCars.count--;
}

int handleControls(GameState* state, SDL_Event* event) {
	if (event->type != SDL_KEYDOWN) return 0;
	switch (event->key.keysym.sym) {
	case SDLK_ESCAPE: state->status = QUIT; return 1;
	case SDLK_n: initializeGameState(state); return 1;
	case SDLK_p: if (state->status == PAUSED) {
		state->status = PLAYING;
		return 1;
	}
	default: return 0;
	}
};

void handleGameplay(GameState* state, SDL_Event* event, Saves* saves) {
	switch (event->type) {
	case SDL_KEYDOWN:
		switch (event->key.keysym.sym) {
		case SDLK_s: state->status = saveGame(state, saves) == 1 ? SAVE_SUCCESS : SAVE_ERROR; break;
		case SDLK_l: createSaveList(saves); state->status = SAVE_SELECTION; break;
		case SDLK_p: state->status = PAUSED; break;
		case SDLK_f: state->status = GAMEOVER; break;
		case SDLK_UP: state->direction.vertical = UP; break;
		case SDLK_DOWN: state->direction.vertical = DOWN; break;
		case SDLK_RIGHT: state->direction.horizontal = RIGHT; break;
		case SDLK_LEFT: state->direction.horizontal = LEFT; break;
		default: break;
		}
		break;
	case SDL_KEYUP:
		if (event->key.keysym.sym == SDLK_UP || event->key.keysym.sym == SDLK_DOWN)
			state->direction.vertical = NONE;
		if ((event->key.keysym.sym == SDLK_RIGHT && state->direction.horizontal == RIGHT) ||
			(event->key.keysym.sym == SDLK_LEFT && state->direction.horizontal == LEFT))
			state->direction.horizontal = STRAIGHT;
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
		if (*selection == saves->count) state->status = PLAYING;
		else state->status = loadGame(state, saves->list[*selection]) == 1 ? LOAD_SUCCESS : LOAD_ERROR;
		*selection = 0;
		break;
	default: break;
	}

	*selection = (*selection + saves->count + 1) % (saves->count + 1);
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

void handleConfirmation(GameState* state, SDL_Event* event, int isError) {
	if (event->type != SDL_KEYDOWN) return;
	if (event->key.keysym.sym == SDLK_RETURN)
		state->status = isError ? QUIT : PLAYING;
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