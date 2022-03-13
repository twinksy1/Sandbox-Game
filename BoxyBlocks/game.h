#pragma once

#include <cmath>
#include <vector>
#include <map>
#include "SDL.h"
#include "blocks.h"
#include "display.h"
#include "Entity.h"
#include "GridMap.h"

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
	std::vector<std::shared_ptr<Block>> blocks;
	std::map<XY, bool> occupiedGridSpots;
	bool gameFreeze = false;

	const Uint32 targetFrameRate = 60;
	Uint32 frameStart = 0;
	Uint32 frameTime = 0;
	Uint32 frameDelay = 0;

	GridMap gridMap;
	short int select = 0;
	float radius = 10.0f;
	bool flush = false;
	bool shift_down = false;
	bool lbutton_down = false;
	bool rbutton_down = false;
	int mousex = 0, mousey = 0;
	int oldMousex = 0, oldMousey = 0;
	XY mouseGridPos;
	bool show_menu = false;
	bool pause = false;
	bool inbounds = false;
	//Chunk chunks[CHUNKSY][CHUNKSX];
	bool debugMode = true;
	int rendChunks = 0;
	Camera cam;
	Display window;
	int curChunkX = 0, curChunkY = 0;

	void pan(int);
	void displayCellInfo(float*);
	bool HandleEvents();
	bool KeyboardEvent(SDL_Event& e);
	void MouseEvent(SDL_Event& e);
	void GenerateBlock();

	void Render();
	void MoveBlocks();
	void CheckForCollisions();
	//Range CoordinatesToMappedRange(double x, double y);
public:
	Game();
	~Game();

	bool Init();
	Errors error = Errors::NONE;
	bool Run();
};

