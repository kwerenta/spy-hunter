#include"./SDL2-2.0.10/include/SDL.h"
#include"./SDL2-2.0.10/include/SDL_main.h"

#define SCREEN_WIDTH	640
#define SCREEN_HEIGHT	480

typedef struct {
	SDL_Renderer* renderer;
	SDL_Window* window;
	SDL_Surface* screen;
	SDL_Texture* screenTexture;
	SDL_Surface* surfaces[2];
	double deltaTime;
} Application;

int initializeApplication(Application* app);

void closeApplication(Application* app);

int loadBMP(Application* app, char* name);