#pragma once

#include <cmath>
#include <vector>
#include <map>
#include "SDL.h"
#include "blocks.h"
#include "display.h"
#include "Entity.h"

const int xres = 640;
const int yres = 480;


class Camera {
	public:
	float focusx;
	float focusy;

	float panSpeed = 10.0f;

	Camera();
};

enum class Errors {
	NONE=0, FONT_LOAD, SDL_WINDOW
};

// Game
class Game {
	std::string title = "Boxy Blocks";
	std::map<std::string, TTF_Font*> fontMap;
	std::vector<Entity> entities;
	bool gameFreeze = false;

	const Uint32 targetFrameRate = 60;
	Uint32 frameStart = 0;
	Uint32 frameTime = 0;
	Uint32 frameDelay = 0;
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
	Errors error = Errors::NONE;
	
	Game();
	~Game();

	bool Init();

	void pan(int);
	void displayCellInfo(float*);
	bool HandleEvents();
	bool KeyboardEvent(SDL_Event& e);
	void MouseEvent(SDL_Event& e);

	void Render();
	void MoveEntities();
	void CheckForCollisions();

	bool Run();
};

