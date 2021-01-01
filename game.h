#ifndef GAME_H
#define GAME_H

#include <cmath>
#include <SDL2/SDL.h>
#include "blocks.h"

const int xres = 1600;
const int yres = 1600;
const int CHUNKSX = 10;
const int CHUNKSY = (yres / MAX_CHUNK_SIZE);
const float CHUNK_REND_DIST = xres * 0.75;

class Camera {
	public:
	float focusx;
	float focusy;

	float panSpeed = 10.0f;

	Camera();
};

// Game
class Game {
	public:
	short int select;
	float radius = 10.0f;
	bool flush = false;
	bool shift_down = false;
	bool lbutton_down = false;
	bool rbutton_down = false;
	int mousex, mousey;
	bool show_menu = false;
	bool pause = false;
	bool inbounds = false;
	Chunk chunks[CHUNKSY][CHUNKSX];
	bool debugMode = true;
	int rendChunks;
	Camera cam;
	int curChunkX, curChunkY;
	
	Game();
	~Game();

	void pan(int);
	void displayCellInfo(float*);
};

#endif