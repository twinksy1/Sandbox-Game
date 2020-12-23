#ifndef BLOCKS_H
#define BLOCKS_H

#include <cmath>

enum {GRIDSIZEX=200, GRIDSIZEY=200};
enum {SAND, WATER, WOOD, FIRE, SMOKE, STEAM, SALT, METAL, NUM_PARTICLES};
// Seconds for fire to spread
const float SPREADTIME = 0.001f;

inline void swap(int &a, int &b)
{
	int tmp = a;
	a = b;
	b = a;
}

inline void swap(float &a, float &b)
{
	float tmp = a;
	a = b;
	b = a;
}
inline float getLength(float* a, float* b)
{
	float xdiff = a[0] - b[0];
	float ydiff = a[1] - b[1];
	return sqrt(xdiff*xdiff + ydiff*ydiff);
}
inline float getLength(int* a, int* b)
{
	float xdiff = a[0] - b[0];
	float ydiff = a[1] - b[1];
	return sqrt(xdiff*xdiff + ydiff*ydiff);
}

// Generic Particle Class
class Particle {
	public:
	float x;
	float y;
	short int id;
	short int vel[2];
	short int acc;
	// Terminal acceleration
	short int Tacc;
	short int color[3];
	bool doKill = false;
	// Reference to cell number
	int cellx;
	int celly;
	int idx;

	Particle() {}
	void initParticle(float x, float y, int id, int cellx, int celly, int idx);
	
	void reinit();

	void operator=(Particle p);

	void printInfo();

};

// Classes for cells & grid
class Cell {
	public:
	int x;
	int y;
	int w;
	int h;
	bool taken;
	int idx;
	int id;

	Cell() {}
	void initCell(int x, int y, int w, int h);
	void operator=(Cell c);
};

class Grid {
	public:
	Cell** cells;
	int max_rows;
	int max_cols;

	Grid();
	~Grid();
	void initGrid(int max_rows, int max_cols, int xres, int yres);
};

#endif