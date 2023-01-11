#include "gui.h"

void renderLegend(Application* app, GameState* state, char* buffer, int bgColor, int borderColor) {
	DrawRectangle(app->screen, 4, 4, SCREEN_WIDTH - 8, 36, borderColor, bgColor);
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

void renderFunctionalities(Application* app, char* buffer, int bgColor, int borderColor) {
	DrawRectangle(app->screen, SCREEN_WIDTH - 84, SCREEN_HEIGHT - 16, 80, 12, borderColor, bgColor);

	sprintf_s(buffer, 128, "a-d, g, i");
	DrawString(app->screen, app->screen->w - 8 - strlen(buffer) * 8, SCREEN_HEIGHT - 14, buffer, app->surfaces[CHARSET_s]);
}

void renderPause(Application* app, char* buffer) {
	sprintf_s(buffer, 128, "PAUSED");
	DrawString(app->screen, SCREEN_WIDTH / 2 - STRING_CENTER(buffer), SCREEN_HEIGHT / 2 - 8, buffer, app->surfaces[CHARSET_s]);

	sprintf_s(buffer, 128, "Press P to continue");
	DrawString(app->screen, SCREEN_WIDTH / 2 - STRING_CENTER(buffer), SCREEN_HEIGHT / 2 + 8, buffer, app->surfaces[CHARSET_s]);
}

void renderBackground(Application* app, int backgroundOffset){
	DrawSurface(app->screen, app->surfaces[GRASS_s], SCREEN_WIDTH / 2, backgroundOffset - SCREEN_HEIGHT / 2);
	DrawSurface(app->screen, app->surfaces[GRASS_s], SCREEN_WIDTH / 2, backgroundOffset + SCREEN_HEIGHT / 2);
}

void renderRoad(Application* app, GameState* state, int backgroundOffset, int color){
	DrawRectangle(app->screen, SCREEN_WIDTH / 2 - state->roadWidth.current / 2, backgroundOffset, state->roadWidth.current, SCREEN_HEIGHT - backgroundOffset, color, color);
	DrawRectangle(app->screen, SCREEN_WIDTH / 2 - state->roadWidth.next / 2, 0, state->roadWidth.next, backgroundOffset, color, color);
}

void renderGameSaveSelection(Application* app, char* buffer, int bgColor, int borderColor, int selectionColor, int selection) {
	DrawRectangle(app->screen, SCREEN_WIDTH / 2 - 150, 44, 300, 300, borderColor, bgColor);

	sprintf_s(buffer, 128, "Select Game Save you want to Load");
	DrawString(app->screen, SCREEN_WIDTH / 2 - STRING_CENTER(buffer), 52, buffer, app->surfaces[CHARSET_s]);

	for (int i = 0; i < app->saves.count; i++)
	{
		sprintf_s(buffer, 128, "Save No. %d: %s", i + 1, app->saves.list[i]);
		if(selection == i)
			DrawRectangle(app->screen, SCREEN_WIDTH / 2 - 146, 66 + i * 12, 292, 12, borderColor, selectionColor);
		DrawString(app->screen, SCREEN_WIDTH / 2 - 142, 68 + i * 12, buffer, app->surfaces[CHARSET_s]);
	}
}