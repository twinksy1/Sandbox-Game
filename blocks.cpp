#include "blocks.h"

#include <cstdio>

void Particle::initParticle(float x, float y, int id, int cellx, int celly, int idx)
{
    this->x = x;
    this->y = y;
    this->id = id;
    this->cellx = cellx;
    this->celly = celly;
    this->idx = idx;

    switch(this->id) {
        case SAND: 
            color[0] = 255;
            color[1] = 255;
            color[2] = 0;
            vel[0] = 1;
            vel[1] = 1;
            acc = 6;
            Tacc = 6;
            break;
        case WATER: 
            color[0] = 0;
            color[1] = 30;
            color[2] = 255;
            vel[0] = 1;
            vel[1] = 1;
            acc = 4;
            Tacc = 4;
            break;
        case WOOD: 
            color[0] = 196;
            color[1] = 154;
            color[2] = 107;
            vel[0] = 0;
            vel[1] = 0;
            acc = 0;
            Tacc = 0;
            break;
        case FIRE: 
            color[0] = 255;
            color[1] = 150;
            color[2] = 0;
            vel[0] = 1;
            vel[1] = 1;
            acc = 0;
            Tacc = 0;
            break;
        case SMOKE: 
            color[0] = 155;
            color[1] = 155;
            color[2] = 155;
            vel[0] = 1;
            vel[1] = 1;
            acc = 0;
            Tacc = 0;
            break;
        case STEAM: 
            color[0] = 180;
            color[1] = 180;
            color[2] = 180;
            vel[0] = 1;
            vel[1] = 1;
            acc = 0;
            Tacc = 0;
            break;
        case SALT:
            color[0] = 255;
            color[1] = 255;
            color[2] = 255;
            vel[0] = 1;
            vel[1] = 1;
            acc = 6;
            Tacc = 6;
            break;
        case METAL: 
            color[0] = 180;
            color[1] = 180;
            color[2] = 180;
            vel[0] = 1;
            vel[1] = 1;
            acc = 0;
            Tacc = 0;
            break;
    }
}
void Particle::reinit()
{
    initParticle(x, y, id, cellx, celly, idx);
}
void Particle::operator=(Particle p)
{
    initParticle(p.x, p.y, p.id, p.cellx, p.celly, p.idx);
    for(int i=0; i<3; i++)
        this->color[i] = p.color[i];
}
void Particle::printInfo()
{
    printf("Particle\n=========\nID: %i, IDX: %i, COLOR: %i, %i, %i,\
            ACC: %i\n\n", id, idx, color[0], color[1], color[2],\
                acc);	
}

void Cell::initCell(int x, int y, int w, int h)
{
    this->x = x;
    this->y = y;
    this->w = w;
    this->h = h;
    taken = false;
    idx = -1;
    id = -1;
}
void Cell::operator=(Cell c)
{
    this->x = c.x;
    this->y = c.y;
    this->w = c.w;
    this->h = c.h;
    this->id = c.id;
    this->idx = c.idx;
    this->taken = c.taken;
}


Grid::Grid() { cells = nullptr; }
Grid::~Grid()
{
    if(cells != nullptr) {
        for(int i=0; i<max_rows; i++) {
            delete cells[i];
            cells[i] = nullptr;
        }
        delete cells;
        cells = nullptr;
    }
}
void Grid::initGrid(int max_rows, int max_cols, int xres, int yres)
{
    this->max_rows = max_rows;
    this->max_cols = max_cols;
    float width = (float)xres / (float)max_cols;
    float height = (float)yres / (float)max_rows;
    
    // Pointers & instantiation
    cells = new Cell *[max_rows];
    for(int i=0; i<max_cols; i++)
        cells[i] = new Cell[max_cols];

    // Initialization
    int x = 0;
    int y = 0;
    for(int i=0; i<max_rows; i++) {
        x = 0;
        for(int j=0; j<max_cols; j++) {
            cells[i][j].initCell(x, y, width, height);
            x += width;
        }
        y += height;
    }
}