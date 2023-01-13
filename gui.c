#include "gui.h"

void renderLegend(Application* app, GameState* state, char* buffer) {
	DrawBorderedRectangle(app->screen, 4, 4, SCREEN_WIDTH - 8, 36, app->colors[OUTLINE], app->colors[MENU]);
	sprintf_s(buffer, 128, "TIME %03.0lf", state->time);
	DrawString(app->screen, 8, 10, buffer, app->surfaces[CHARSET_s]);

	if (state->immortalityTime > 0)
		sprintf_s(buffer, 128, "IMMORTAL %02.0lf", state->immortalityTime);
	else
		sprintf_s(buffer, 128, "CARS %02d", state->spareCars.count);
	DrawString(app->screen, 8, 26, buffer, app->surfaces[CHARSET_s]);

	sprintf_s(buffer, 128, "SCORE %05d", state->score);
	DrawString(app->screen, app->screen->w - 8 - strlen(buffer) * 8, 10, buffer, app->surfaces[CHARSET_s]);

	sprintf_s(buffer, 128, "Kamil Wenta 193437");
	DrawString(app->screen, app->screen->w / 2 - STRING_CENTER(buffer), 26, buffer, app->surfaces[CHARSET_s]);
}

void renderFunctionalities(Application* app, char* buffer) {
	DrawBorderedRectangle(app->screen, SCREEN_WIDTH - 84, SCREEN_HEIGHT - 16, 80, 12, app->colors[OUTLINE], app->colors[MENU]);

	sprintf_s(buffer, 128, "a-i, m, o");
	DrawString(app->screen, app->screen->w - 8 - strlen(buffer) * 8, SCREEN_HEIGHT - 14, buffer, app->surfaces[CHARSET_s]);
}

void renderPause(Application* app, char* buffer) {
	sprintf_s(buffer, 128, "PAUSED");
	DrawString(app->screen, SCREEN_WIDTH / 2 - STRING_CENTER(buffer), SCREEN_HEIGHT / 2 - 8, buffer, app->surfaces[CHARSET_s]);

	sprintf_s(buffer, 128, "Press P to continue");
	DrawString(app->screen, SCREEN_WIDTH / 2 - STRING_CENTER(buffer), SCREEN_HEIGHT / 2 + 8, buffer, app->surfaces[CHARSET_s]);
}

void renderBackground(Application* app, int backgroundOffset) {
	DrawSurface(app->screen, app->surfaces[BACKGROUND_s], SCREEN_WIDTH / 2, backgroundOffset - SCREEN_HEIGHT / 2);
	DrawSurface(app->screen, app->surfaces[BACKGROUND_s], SCREEN_WIDTH / 2, backgroundOffset + SCREEN_HEIGHT / 2);
}

void renderRoad(Application* app, GameState* state) {
	DrawRectangle(app->screen, SCREEN_WIDTH / 2 - state->roadWidth.current / 2, state->backgroundOffset, state->roadWidth.current, SCREEN_HEIGHT - state->backgroundOffset, app->colors[ROAD]);
	DrawRectangle(app->screen, SCREEN_WIDTH / 2 - state->roadWidth.next / 2, 0, state->roadWidth.next, state->backgroundOffset, app->colors[ROAD]);

	DrawSurface(app->screen, app->surfaces[ROAD_LINES_s], SCREEN_WIDTH / 2, state->backgroundOffset - SCREEN_HEIGHT / 2);
	DrawSurface(app->screen, app->surfaces[ROAD_LINES_s], SCREEN_WIDTH / 2, state->backgroundOffset + SCREEN_HEIGHT / 2);
}

void renderAI(Application* app, GameState* state) {
	for (int i = 0; i < AI_CARS_COUNT; i++)
	{
		if (state->aiCars[i].hp == 0) continue;
		DrawSurface(app->screen, app->surfaces[state->aiCars[i].type == NON_ENEMY ? NON_ENEMY_CAR_s : ENEMY_CAR_s], SCREEN_WIDTH / 2 + state->aiCars[i].position.x, state->aiCars[i].position.y);
	}
}

void renderGameSaveSelection(Application* app, char* buffer, int selection) {
	DrawBorderedRectangle(app->screen, SCREEN_WIDTH / 2 - 150, 44, 300, app->saves.count * 12 + 56, app->colors[OUTLINE], app->colors[MENU]);

	sprintf_s(buffer, 128, "Select Game Save you want to Load");
	DrawString(app->screen, SCREEN_WIDTH / 2 - STRING_CENTER(buffer), 52, buffer, app->surfaces[CHARSET_s]);

	for (int i = 0; i < app->saves.count; i++)
	{
		sprintf_s(buffer, 128, "Save No. %d: %s", i + 1, app->saves.list[i]);
		if (selection == i)
			DrawBorderedRectangle(app->screen, SCREEN_WIDTH / 2 - 146, 66 + i * 12, 292, 12, app->colors[OUTLINE], app->colors[ROAD]);
		DrawString(app->screen, SCREEN_WIDTH / 2 - 142, 68 + i * 12, buffer, app->surfaces[CHARSET_s]);
	}

	sprintf_s(buffer, 128, "Cancel");
	if (selection == app->saves.count)
		DrawBorderedRectangle(app->screen, SCREEN_WIDTH / 2 - 146, 66 + (app->saves.count + 1) * 12, 292, 12, app->colors[OUTLINE], app->colors[ROAD]);
	DrawString(app->screen, SCREEN_WIDTH / 2 - 142, 68 + (app->saves.count + 1) * 12, buffer, app->surfaces[CHARSET_s]);
}

void renderGameOver(Application* app, GameState* state, char* buffer, int selection) {
	DrawBorderedRectangle(app->screen, SCREEN_WIDTH / 2 - 150, 44, 300, 80, app->colors[OUTLINE], app->colors[MENU]);

	sprintf_s(buffer, 128, "GAME OVER");
	DrawString(app->screen, SCREEN_WIDTH / 2 - STRING_CENTER(buffer), 52, buffer, app->surfaces[CHARSET_s]);
	sprintf_s(buffer, 128, "Your result:");
	DrawString(app->screen, SCREEN_WIDTH / 2 - STRING_CENTER(buffer), 64, buffer, app->surfaces[CHARSET_s]);

	sprintf_s(buffer, 128, "TIME: %.2lf", state->time);
	DrawString(app->screen, SCREEN_WIDTH / 2 - 142, 76, buffer, app->surfaces[CHARSET_s]);
	sprintf_s(buffer, 128, "SCORE: %d", state->score);
	DrawString(app->screen, SCREEN_WIDTH / 2 + 142 - strlen(buffer) * 8, 76, buffer, app->surfaces[CHARSET_s]);

	sprintf_s(buffer, 128, "Do you want to save your result?");
	DrawString(app->screen, SCREEN_WIDTH / 2 - STRING_CENTER(buffer), 92, buffer, app->surfaces[CHARSET_s]);

	DrawBorderedRectangle(app->screen, SCREEN_WIDTH / 2 + (selection == 0 ? -16 - 3 * 8 : 8), 102, (selection == 0 ? 3 * 8 + 8 : 2 * 8 + 8), 12, app->colors[OUTLINE], app->colors[ROAD]);
	sprintf_s(buffer, 128, "Yes");
	DrawString(app->screen, SCREEN_WIDTH / 2 - 12 - strlen(buffer) * 8, 104, buffer, app->surfaces[CHARSET_s]);
	sprintf_s(buffer, 128, "No");
	DrawString(app->screen, SCREEN_WIDTH / 2 + 12, 104, buffer, app->surfaces[CHARSET_s]);
}

void renderScoreboard(Application* app, GameState* state, char* buffer) {
	DrawBorderedRectangle(app->screen, SCREEN_WIDTH / 2 - 150, 44, 300, app->scoreboard.count * 12 + 80, app->colors[OUTLINE], app->colors[MENU]);

	sprintf_s(buffer, 128, "SCOREBOARD");
	DrawString(app->screen, SCREEN_WIDTH / 2 - STRING_CENTER(buffer), 52, buffer, app->surfaces[CHARSET_s]);

	for (int i = 0; i < app->scoreboard.count; i++)
	{
		sprintf_s(buffer, 128, "%d. TIME: %.2lf, SCORE: %d", i + 1, app->scoreboard.list[i].time, app->scoreboard.list[i].score);
		DrawString(app->screen, SCREEN_WIDTH / 2 - 142, 68 + i * 12, buffer, app->surfaces[CHARSET_s]);
	}

	sprintf_s(buffer, 128, "t - Sort by time");
	DrawString(app->screen, SCREEN_WIDTH / 2 - 142, 90 + app->scoreboard.count * 12, buffer, app->surfaces[CHARSET_s]);
	sprintf_s(buffer, 128, "p - Sort by points");
	DrawString(app->screen, SCREEN_WIDTH / 2 + 142 - strlen(buffer) * 8, 90 + app->scoreboard.count * 12, buffer, app->surfaces[CHARSET_s]);

	sprintf_s(buffer, 128, "Press ENTER to start new game");
	DrawString(app->screen, SCREEN_WIDTH / 2 - STRING_CENTER(buffer), 102 + app->scoreboard.count * 12, buffer, app->surfaces[CHARSET_s]);
}

void renderConfirmation(Application* app, char* buffer, GameStatus status) {
	DrawBorderedRectangle(app->screen, SCREEN_WIDTH / 2 - 150, SCREEN_HEIGHT / 2 - 20, 300, 40, app->colors[OUTLINE], app->colors[MENU]);

	int isError = status == SAVE_ERROR || status == LOAD_ERROR;
	if(isError) sprintf_s(buffer, 128, "Game wasn't %s correctly.", status == SAVE_ERROR ? "saved" : "loaded");
	else sprintf_s(buffer, 128, "Game has been successfully %s.", status == SAVE_SUCCESS ? "saved" : "loaded");
	DrawString(app->screen, SCREEN_WIDTH / 2 - STRING_CENTER(buffer), SCREEN_HEIGHT / 2 - 12, buffer, app->surfaces[CHARSET_s]);
	sprintf_s(buffer, 128, isError ? "Press ENTER to close game" : "Press ENTER to continue");
	DrawString(app->screen, SCREEN_WIDTH / 2 - STRING_CENTER(buffer), SCREEN_HEIGHT / 2 + 4, buffer, app->surfaces[CHARSET_s]);
}