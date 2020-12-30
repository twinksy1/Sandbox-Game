#ifndef DISPLAY_H
#define DISPLAY_H


#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <cstdio>
#include <string>

#include "blocks.h"
#include "handleInput.h"
#include "game.h"

class Display {
	private:
	SDL_Window* window;
	SDL_Surface* screen_surface;
	SDL_Renderer* rend;
	SDL_Texture* message[NUM_PARTICLES];
	SDL_Texture* blockTextures[NUM_PARTICLES];
	public:

	Display();
	~Display();
	int init();
	void setColor(int, int, int);
	void preRender();
	void postRender();
	// Drawing functions
	void drawLine(int, int, int, int);
	void drawPoint(int, int);
	void drawRect(int, int, int, int);
	void fillRect(int, int, int, int);
	void loadTextures();
	void updateTextures();
	void drawTexture(int, int, int);
	void drawText(int, int, int, int, const char*, int, int);
    bool handleInput(Game&);
};

#endif