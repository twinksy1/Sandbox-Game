#pragma once

#include <cmath>
#include "SDL.h"
#include "blocks.h"
#include "display.h"

const int xres = 640;
const int yres = 480;


class Camera {
	public:
	float focusx;
	float focusy;

	float panSpeed = 10.0f;

	Camera();
};

// Game
class Game {
	std::string title = "Boxy Blocks";
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
	//Chunk chunks[CHUNKSY][CHUNKSX];
	bool debugMode = true;
	int rendChunks;
	Camera cam;
	Display window;
	int curChunkX, curChunkY;
	
	Game();
	~Game();

	bool Init();

	void pan(int);
	void displayCellInfo(float*);
	bool HandleEvents();
	bool KeyboardEvent(SDL_Event& e);
	void MouseEvent(SDL_Event& e);
	void Render();
};

