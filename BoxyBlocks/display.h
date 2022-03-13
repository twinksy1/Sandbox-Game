#pragma once


#include "SDL.h"
#include "SDL_ttf.h"
#include "SDL_image.h"
#include "Square.h"

#include <cstdio>
#include <string>
#include <utility>
using namespace Shapes2D;

class Display {
	SDL_Window* window;
	SDL_Surface* screenSurface;
	SDL_Renderer* rend;
	//SDL_Texture* message[NUM_PARTICLES];
	//SDL_Texture* blockTextures[NUM_PARTICLES];
	//SDL_Texture* bg;

	Uint32 currentColor = 0;

	struct SDL_Colors {
		SDL_Color WHITE = { 255, 255, 255 };
		SDL_Color BLACK = { 0, 0, 0 };
		SDL_Color RED = { 255, 0, 0 };
		SDL_Color GREEN = { 0, 255, 0 };
		SDL_Color BLUE = { 0, 0, 255 };
	};

public:
	SDL_Colors SDL_COLORS;

	Display();
	~Display();
	int Init(int xres, int yres, std::string title = "DEFAULT TITLE");
	void SetColor(int r, int g, int b);
	void SetColor(SDL_Color color);
	void PreRender();
	void PostRender();

	std::wstring ExePath();
	// Drawing functions
	void DrawLine(int x1, int y1, int x2, int y2);
	void DrawLine(Point p1, Point p2);
	void DrawLine(Line line);

	void DrawPoint(int x, int y);
	void DrawPoint(Point pt);

	SDL_Rect CreateRect(int x, int y, int w, int h);
	SDL_Rect CreateRect(float x, float y, float w, float h);
	SDL_FRect CreateRectF(double x, double y, double w, double h);
	SDL_FRect CreateRectF(float x, float y, float w, float h);
	void DrawRect(int x, int y, int w, int h);
	void DrawRect(double x, double y, double w, double h);
	void DrawRect(float x, float y, float w, float h);
	void DrawRect(std::pair<float, float> pos, std::pair<float, float> dimmensions);
	void DrawRect(std::pair<double, double> pos, std::pair<double, double> dimmensions);
	void DrawRect(Square square);
	void FillRect(int x, int y, int w, int h);
	void FillRect(float x, float y, float w, float h);
	void FillRect(double x, double y, double w, double h);
	void FillRect(std::pair<float, float> pos, std::pair<float, float> dimmensions);
	void FillRect(std::pair<double, double> pos, std::pair<double, double> dimmensions);
	void FillRect(XY pt, XY dimmensions);
	void FillRect(Point pt, XY dimmensions);
	void FillRect(Point pt, std::pair<double, double> dimmensions);
	void FillRect(Square square);

	void LoadTextures();
	void UpdateTextures();
	void DrawTexture(int, int, int);
	void DrawBackground(int, int, int, int);

	void DrawScreenText(int x, int y, int w, int h, TTF_Font* font, std::string text, SDL_Color color = { 255, 255, 255 });
	void DrawScreenText(float x, float y, float w, float h, TTF_Font* font, std::string text, SDL_Color color = { 255, 255, 255 });
	void DrawScreenText(double x, double y, double w, double h, TTF_Font* font, std::string text, SDL_Color color = { 255, 255, 255 });
	void DrawScreenText(std::pair<float, float> pos, std::pair<float, float> dimmensions, TTF_Font* font, std::string text, SDL_Color color = { 255, 255, 255 });
	void DrawScreenText(std::pair<double, double> pos, std::pair<double, double> dimmensions, TTF_Font* font, std::string text, SDL_Color color = { 255, 255, 255 });
};

