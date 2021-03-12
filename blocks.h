#ifndef BLOCKS_H
#define BLOCKS_H

#include <cmath>
#include <string>
#include <fstream>

const int P_SIZE = 32;
const int MAX_CHUNK_SIZE = 800;

enum {GRIDSIZEX=P_SIZE, GRIDSIZEY=P_SIZE};
enum {AIR=-1, SAND=0, WATER, WOOD, FIRE, SMOKE, STEAM, SALT, METAL, STONE, DIRT, GRASS, NUM_PARTICLES};
const std::string BLOCK_TYPES[NUM_PARTICLES] = {"SAND", "WATER", "WOOD", "FIRE", "SMOKE",
												"STEAM", "SALT", "METAL", "STONE", "DIRT",
												"GRASS"
											   };

inline float getDist(float a[2], float b[2])
{
	float xdiff = a[0] - b[0];
	float ydiff = a[1] - b[1];
	return sqrt(xdiff*xdiff + ydiff*ydiff);
}

inline void swap(short int &a, short int &b)
{
	short int tmp = a;
	a = b;
	b = a;
}

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
	int cellRow;
	int cellCol;
	int instances;

	Cell* down;
	Cell* up;
	Cell* left;
	Cell* right;

	Cell();
	void initCell(int, int, int, int, int, int);
	void operator=(Cell);
};

class Grid {
	public:
	Cell** cells;
	int max_rows;
	int max_cols;

	Grid();
	~Grid();
	void initGrid(float, float);
};

class Chunk {
	public:
	Grid g;
	Particle* blocks;
	int blockCount;
	float startX;
	float startY;
	int chunkID;
	bool doRender;

	Chunk* up;
	Chunk* down;
	Chunk* left;
	Chunk* right;

	Chunk();
	~Chunk();

	void initChunk(int, float, float);
	void loadChunk();
	bool allocateBlock(int, int, int);
	bool allocateBlock(Particle&, int, int);
	bool deallocateBlocks();
	bool deallocateBlocks(float*);
	bool deallocateBlock(int);
	void unoccupyCell(int, int);
	void refreshGrid();
	void updateCellInfo(int idx);
	bool within(float, float);
	void updateBlocks();
	void blockPhysics();
};

#endif
