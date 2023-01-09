#include "gui.h"

void renderLegend(Application* app, GameState* state, char* buffer, int bgColor, int borderColor) {
	DrawRectangle(app->screen, 4, 4, SCREEN_WIDTH - 8, 36, borderColor, bgColor);
	sprintf_s(buffer, 128, "TIME %03.0lf", state->time);
	DrawString(app->screen, 8, 10, buffer, app->surfaces[CHARSET_s]);

	sprintf_s(buffer, 128, "SCORE %05d", state->score);
	DrawString(app->screen, app->screen->w - 8 - strlen(buffer) * 8, 10, buffer, app->surfaces[CHARSET_s]);

	sprintf_s(buffer, 128, "Kamil Wenta 193437");
	DrawString(app->screen, app->screen->w / 2 - STRING_CENTER(buffer), 26, buffer, app->surfaces[CHARSET_s]);
}

void renderPause(Application* app, char* buffer) {
	sprintf_s(buffer, 128, "PAUSED");
	DrawString(app->screen, SCREEN_WIDTH / 2 - STRING_CENTER(buffer), SCREEN_HEIGHT / 2 - 8, buffer, app->surfaces[CHARSET_s]);

	sprintf_s(buffer, 128, "Press P to continue");
	DrawString(app->screen, SCREEN_WIDTH / 2 - STRING_CENTER(buffer), SCREEN_HEIGHT / 2 + 8, buffer, app->surfaces[CHARSET_s]);
}