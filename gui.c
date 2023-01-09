#include "gui.h"

void renderLegend(Application* app, GameState* state, char* buffer, int bgColor, int borderColor) {
	DrawRectangle(app->screen, 4, 4, SCREEN_WIDTH - 8, 36, borderColor, bgColor);
	sprintf_s(buffer, 128, "TIME %03.0lf", state->time);
	DrawString(app->screen, 8, 10, buffer, app->surfaces[CHARSET_s]);

	sprintf_s(buffer, 128, "SCORE %05d", state->score);
	DrawString(app->screen, app->screen->w - 8 - strlen(buffer) * 8, 10, buffer, app->surfaces[CHARSET_s]);

	sprintf_s(buffer, 128, "Kamil Wenta 193437");
	DrawString(app->screen, app->screen->w / 2 - strlen(buffer) * 8 / 2, 26, buffer, app->surfaces[CHARSET_s]);
}