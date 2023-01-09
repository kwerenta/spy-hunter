typedef enum {
	PLAYING,
	QUIT
} GameStatus;

typedef struct {
	int score;
	double position;
	int haltScore;
	int carsLeft;
	double time;
	double distance;
	double speed;
	GameStatus status;
} GameState;

void initializeGameState(GameState* state);
