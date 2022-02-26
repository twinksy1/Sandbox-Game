#pragma once


#include "SDL.h"
#include "SDL_ttf.h"
#include "SDL_image.h"

#include <cstdio>
#include <string>

class Display {
	private:
	SDL_Window* window;
	SDL_Surface* screen_surface;
	SDL_Renderer* rend;
	//SDL_Texture* message[NUM_PARTICLES];
	//SDL_Texture* blockTextures[NUM_PARTICLES];
	//SDL_Texture* bg;
	public:

	Display();
	~Display();
	int Init(int xres, int yres, std::string title = "DEFAULT TITLE");
	void SetColor(int, int, int);
	void PreRender();
	void PostRender();
	// Drawing functions
	void DrawLine(int, int, int, int);
	void DrawPoint(int, int);
	void DrawRect(int, int, int, int);
	void FillRect(int, int, int, int);
	void LoadTextures();
	void UpdateTextures();
	void DrawTexture(int, int, int);
	void DrawBackground(int, int, int, int);
	void DrawText(int, int, int, int, const char*, int, int);
};

