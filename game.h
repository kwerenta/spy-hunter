typedef enum {
	PLAYING
} GameStatus;

typedef struct {
	int score;
	int position;
	int haltScore;
	int carsLeft;
	double time;
	double distance;
	double speed;
	GameStatus status;
} GameState;

void initializeGameState(GameState* state);
