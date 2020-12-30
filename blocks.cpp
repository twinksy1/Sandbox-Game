#include "blocks.h"

#include <cstdio>

void updateWater(int, Chunk&);
void updateSand(int, Chunk&);

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
            vel[0] = 1;
            vel[1] = 1;
            acc = 2;
            Tacc = 2;
            break;
        case WATER: 
            vel[0] = 1;
            vel[1] = 1;
            acc = 2;
            Tacc = 2;
            break;
        case WOOD: 
            vel[0] = 0;
            vel[1] = 0;
            acc = 0;
            Tacc = 0;
            break;
        case FIRE: 
            vel[0] = 1;
            vel[1] = 1;
            acc = 0;
            Tacc = 0;
            break;
        case SMOKE: 
            vel[0] = 1;
            vel[1] = 1;
            acc = 0;
            Tacc = 0;
            break;
        case STEAM: 
            vel[0] = 1;
            vel[1] = 1;
            acc = 0;
            Tacc = 0;
            break;
        case SALT:
            vel[0] = 1;
            vel[1] = 1;
            acc = 6;
            Tacc = 6;
            break;
        case METAL: 
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
void Particle::modify(int newID)
{
    this->id = newID;
}
void Particle::swap(Particle& other)
{
    Particle tmp = other;
    other = *this;
    *this = tmp;
}
void Particle::operator=(Particle p)
{
    initParticle(p.x, p.y, p.id, p.cellx, p.celly, p.idx);
	this->vel[0] = p.vel[0];
	this->vel[1] = p.vel[1];
}
void Particle::printInfo()
{
    printf("Particle\n=========\nID: %i, IDX: %i, ACC: %i\n\n", id, idx, acc);
}

Cell::Cell()
{
	left = right = down = up = nullptr;
}
void Cell::initCell(int x, int y, int w, int h, int r, int c)
{
    this->x = x;
    this->y = y;
    this->w = w;
    this->h = h;
	cellCol = c;
	cellRow = r;
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


Grid::Grid() 
{ 
    cells = nullptr; 
    max_rows = GRIDSIZEY;
    max_cols = GRIDSIZEX;
}
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
void Grid::initGrid(float startX, float startY)
{
    float width = GRIDSIZEX;
    float height = GRIDSIZEY;
	max_rows = MAX_CHUNK_SIZE / height;
	max_cols = MAX_CHUNK_SIZE / width;

    if(cells != nullptr) {
        for(int i=0; i<max_rows; i++) {
            delete cells[i];
            cells[i] = nullptr;
        }
        delete cells;
        cells = nullptr;
    }
    
    // Pointers & instantiation
    cells = new Cell *[max_rows];
    for(int i=0; i<max_rows; i++)
        cells[i] = new Cell[max_cols];

    // Initialization
    int x = startX;
    int y = startY;
    for(int i=0; i<max_rows; i++) {
        x = startX;
        for(int j=0; j<max_cols; j++) {
            cells[i][j].initCell(x, y, width, height, i, j);
			if(j > 0) {
				cells[i][j].left = &cells[i][j-1];
			}
			if(j < max_cols-1) {
				cells[i][j].right = &cells[i][j+1];
			}
			if(i > 0) {
				cells[i][j].up = &cells[i-1][j];
			}
			if(i < max_rows-1) {
				cells[i][j].down = &cells[i+1][j];
			}
            x += width;
        }
        y += height;
    }
}

Chunk::Chunk()
{
    blocks = nullptr;
    blockCount = 0;
    doRender = false;
}
Chunk::~Chunk()
{
    if(blocks != nullptr) {
        delete blocks;
        blocks = nullptr;
    }
}
void Chunk::initChunk(int id, float startX, float startY)
{
    this->chunkID = id;
    g.initGrid(startX, startY);
    this->startX = startX;
    this->startY = startY;
}
void Chunk::loadChunk()
{
    float width = GRIDSIZEX;
    float height = GRIDSIZEY;
    int x = startX;
    int y = startY;
    for(int i=0; i<g.max_rows; i++) {
        x = startX;
        for(int j=0; j<g.max_cols; j++) {
            g.cells[i][j].x = x;
            g.cells[i][j].y = y;
            x += width;
        }
        y += height;
    }
    updateBlocks();
}
bool Chunk::allocateBlock(Particle& p, int cellx, int celly)
{
	if(blockCount < 1) {
		try {
			blocks = new Particle;
			*blocks = p;
			blocks->cellx = cellx;
			blocks->celly = celly;
			g.cells[celly][cellx].idx = blocks->idx;
			g.cells[celly][cellx].id = blocks->id;
			g.cells[celly][cellx].taken = true;
			blockCount = 1;
            return true;
		} catch(int BAD_ALLOC) {
			printf("ERROR: failed to allocate block in chunk #%i\n", chunkID);
            return false;
		}
	} else {
		try {
			Particle* tmp = blocks;
			blocks = new Particle[blockCount+1];
			for(int i=0; i<blockCount; i++)
				blocks[i] = tmp[i];
			blocks[blockCount] = p;
			blocks[blockCount].cellx = cellx;
			blocks[blockCount].celly = celly;
			g.cells[celly][cellx].taken = true;
			g.cells[celly][cellx].idx = blocks[blockCount].idx;
			g.cells[celly][cellx].id = blocks[blockCount].id;
			delete tmp;
			tmp = nullptr;
			blockCount++;
            return true;
		} catch(int BAD_ALLOC) {
			printf("ERROR: failed to allocate block in chunk #%i\n", chunkID);
            return false;
		}
	}
}
bool Chunk::allocateBlock(int id, int x, int y)
{
    int cellx=0, celly=0;
    float mouse[2] = {(float)x, (float)y};
    float closest_dist = 999.99f;

    for(int i=0; i<g.max_rows; i++) {
        for(int j=0; j<g.max_cols; j++) {
            float cell_pos[2] = {(float)g.cells[i][j].x, 
                (float)g.cells[i][j].y};
            float dist = getDist(mouse, cell_pos);
            if(dist < closest_dist && !g.cells[i][j].taken) {
                closest_dist = dist;
                celly = i;
                cellx = j;
            }
        }
    }
    if(closest_dist > 25.0f) return false;
    x = g.cells[celly][cellx].x;
    y = g.cells[celly][cellx].y;

	if(blockCount < 1) {
		try {
			blocks = new Particle;
			blocks->initParticle(x, y, id, cellx, celly, blockCount);
			g.cells[celly][cellx].idx = blocks->idx;
			g.cells[celly][cellx].id = blocks->id;
			g.cells[celly][cellx].taken = true;
			blockCount = 1;
            return true;
		} catch(int BAD_ALLOC) {
			printf("ERROR: failed to allocate block in chunk #%i\n", chunkID);
            return false;
		}
	} else {
		try {
			Particle* tmp = blocks;
			blocks = new Particle[blockCount+1];
			for(int i=0; i<blockCount; i++)
				blocks[i] = tmp[i];
			blocks[blockCount].initParticle(x, y, id, cellx, celly, blockCount);
			g.cells[celly][cellx].taken = true;
			g.cells[celly][cellx].idx = blocks[blockCount].idx;
			g.cells[celly][cellx].id = blocks[blockCount].id;
			delete tmp;
			tmp = nullptr;
			blockCount++;
            return true;
		} catch(int BAD_ALLOC) {
			printf("ERROR: failed to allocate block in chunk #%i\n", chunkID);
            return false;
		}
	}
}
bool Chunk::deallocateBlocks()
{
	int kill_count = 0;
	for(int i=0; i<blockCount; i++)
		if(blocks[i].doKill) {
			kill_count++;
			int xpos = blocks[i].cellx;
			int ypos = blocks[i].celly;
			g.cells[ypos][xpos].taken = false;
			g.cells[ypos][xpos].idx = -1;
			g.cells[ypos][xpos].id = -1;
		}
	if(kill_count == 0) return true;
	else if(blockCount - kill_count <= 0) {
		try {
			delete blocks;
			blocks = nullptr;
			blockCount = 0;
            return true;
		} catch(int err) {
			printf("ERROR: failed to deallocate block in chunk #%i\n", chunkID);
            return false;
		}
	} else {
		try {
			Particle* tmp = blocks;
			int new_count = blockCount - kill_count;
			blocks = new Particle[new_count];
			int j=0;
			for(int i=0; i<blockCount; i++) {
				if(!tmp[i].doKill && j < new_count) {
					// Copy if no kill order
					blocks[j] = tmp[i];
					j++;
				}
			}

			delete tmp;
			tmp = nullptr;
			blockCount = new_count;
            return true;
		} catch(int err) {
			printf("ERROR: failed to deallocate block in chunk #%i\n", chunkID);
            return false;
		}
	}
}
void Chunk::updateBlocks()
{
    Particle* cur;
    for(int i=0; i<blockCount; i++) {
        cur = &blocks[i];
        cur->x = g.cells[cur->celly][cur->cellx].x;
        cur->y = g.cells[cur->celly][cur->cellx].y;
    }
}
void Chunk::blockPhysics()
{
	for(int i=0; i<blockCount; i++) {
		// Particle movement & interactions
		switch(blocks[i].id) {
			case SAND:
				updateSand(i, *this);
			 	break;
			case WATER:
				updateWater(i, *this);
				break;
			// case WOOD:
			// 	updateWood(i);
			// 	break;
			// case FIRE: {
			// 	// Don't let fire burn instantly
			// 	auto now = std::chrono::high_resolution_clock::now();
			// 	std::chrono::duration<double, std::milli> elapsed = now - time_since_burn;
			// 	if(elapsed.count() > SPREADTIME) {
			// 		updateFire(i);
			// 		time_since_burn = std::chrono::high_resolution_clock::now();
			// 	}
			// 	break;
			// 	   }
			// case SMOKE:
			// 	updateSmoke(i);
			// 	break;
			// case STEAM:
			// 	updateSteam(i);
			// 	break;
			// case SALT:
			// 	updateSalt(i);
			// 	break;
			// case METAL:
			// 	updateMetal(i);
			// 	break;
		}
	}
	deallocateBlocks();
}
void Chunk::updateCellInfo(int idx)
{
	int cellx = blocks[idx].cellx;
	int celly = blocks[idx].celly;

	g.cells[celly][cellx].taken = true;
	g.cells[celly][cellx].id = blocks[idx].id;
	g.cells[celly][cellx].idx = idx;
	blocks[idx].x = g.cells[celly][cellx].x;
	blocks[idx].y = g.cells[celly][cellx].y;
}
bool Chunk::within(float x, float y)
{
    if(x >= startX && x <= startX + MAX_CHUNK_SIZE && y >= startY && y <= startY + MAX_CHUNK_SIZE) {
        return true;
    }
    return false;
}

// Begin - How each block type reacts with its environment


void updateWater(int idx, Chunk& c)
{
	c.updateCellInfo(idx);

	Particle* block = &c.blocks[idx];
	const short int ACC = block->Tacc;
	// Position of current cell
	int posx = block->cellx;
	int posy = block->celly;

	bool isFalling = true;
	Cell* curCell = &c.g.cells[posy][posx];
	if(curCell->down != nullptr) {
		if(curCell->down->taken) {
			isFalling = false;
		}
	} else {
		isFalling = false;
	}

	int dir = rand() % 2;

	// Default water physics
	if(isFalling) {
		int i = 0;
		Cell* tmp = curCell;
		while(tmp->down != nullptr && i < block->vel[1]) {
			if(tmp->down->taken) break;
			tmp = tmp->down;
			i++;
		}

		if(curCell == tmp) {
			block->vel[1] = 1;
		} else {
			if(block->vel[1] < ACC) block->vel[1]++;

			c.g.cells[posy][posx].taken = false;
			if(posy + i >= c.g.max_rows) {
				c.down->allocateBlock(block->id, block->x, block->y);
				block->doKill = true;
				return;
			} else {
				block->celly = posy + i;
				c.updateCellInfo(idx);
				curCell = tmp;
			}
		}
	} else {
		if(dir == 0) {
			if(curCell->left != nullptr) {
				if(!curCell->left->taken) {
					c.g.cells[posy][posx].taken = false;
					if(posx > 0) {
						block->cellx = posx - 1;
						c.updateCellInfo(idx);
					} else {
						c.left->allocateBlock(*block, curCell->left->cellCol, curCell->left->cellRow);
						block->doKill = true;
						return;
					}
				}
			}
		} else {
			if(curCell->right != nullptr) {
				if(!curCell->right->taken) {
					c.g.cells[posy][posx].taken = false;
					if(posx != c.g.max_cols-1) {
						block->cellx = posx + 1;
						c.updateCellInfo(idx);
					} else {
						c.right->allocateBlock(block->id, block->x, block->y);
						block->doKill = true;
						return;
					}
				}
			}
		}
	}

	// Handling environment (other blocks)
}

void updateSand(int idx, Chunk& c)
{
	c.updateCellInfo(idx);

	Particle* block = &c.blocks[idx];
	const short int ACC = block->Tacc;
	// Position of current cell
	int posx = block->cellx;
	int posy = block->celly;

	bool isFalling = true;
	bool striveR = false;
	bool striveL = false;

	Cell* curCell = &c.g.cells[posy][posx];
	if(curCell->down != nullptr) {
		if(curCell->down->taken) {
			isFalling = false;
			bool foundSpot = false;
			if(curCell->down->left != nullptr) {
				if(!curCell->down->left->taken) {
					foundSpot = true;
					striveL = true;
					striveR = false;
				}
			}
			if(curCell->down->right != nullptr && !foundSpot) {
				if(!curCell->down->right->taken) {
					striveL = false;
					striveR = true;
				}
			}
		}
	} else {
		isFalling = false;
	}

	// Default sand physics
	if(isFalling) {
		int i = 0;
		Cell* tmp = curCell;
		while(tmp->down != nullptr && i < block->vel[1]) {
			if(tmp->down->taken) break;
			tmp = tmp->down;
			i++;
		}

		if(curCell == tmp) {
			block->vel[1] = 1;
		} else {
			if(block->vel[1] < ACC) block->vel[1]++;

			c.g.cells[posy][posx].taken = false;
			if(posy + i >= c.g.max_rows) {
				c.down->allocateBlock(*block, tmp->cellCol, tmp->cellRow);
				block->doKill = true;
				return;
			} else {
				block->celly = posy + i;
				c.updateCellInfo(idx);
				curCell = tmp;
			}
		}
	} else if(striveL) {
		c.g.cells[posy][posx].taken = false;
		// Bottom left corner
		if(posy + 1 >= c.g.max_rows && posx - 1 < 0) {
			c.down->left->allocateBlock(block->id, block->x, block->y);
			block->doKill = true;
			return;
		}
		// Bottom
		else if(posy + 1 >= c.g.max_rows) {
			c.down->allocateBlock(block->id, block->x, block->y);
			block->doKill = true;
			return;
		} else if(posx - 1 < 0) {
			c.left->allocateBlock(block->id, block->x, block->y);
			block->doKill = true;
			return;
		} else {
			// Left
			block->celly++;
			block->cellx--;
			c.updateCellInfo(idx);
			curCell = curCell->down->left;
		}
	} else if(striveR) {
		c.g.cells[posy][posx].taken = false;
		// Bottom right corner
		if(posy + 1 >= c.g.max_rows && posx + 1 >= c.g.max_cols) {
			c.down->right->allocateBlock(block->id, block->x, block->y);
			block->doKill = true;
			return;
		}
		// Bottom
		else if(posy + 1 >= c.g.max_rows) {
			c.down->allocateBlock(block->id, block->x, block->y);
			block->doKill = true;
			return;
		} else if(posx + 1 >= c.g.max_cols) {
			c.right->allocateBlock(block->id, block->x, block->y);
			block->doKill = true;
			return;
		} else {
			// Left
			block->celly++;
			block->cellx++;
			c.updateCellInfo(idx);
			curCell = curCell->down->right;
			return;
		}
	}

	// Block environment reactions

	// ------With water
	if(curCell->down != nullptr) {
		if(curCell->down->taken && curCell->down->id == WATER) {
			// Both pass chunks
			if(block->celly + 1 >= c.g.max_rows) {
				Particle* waterBlock = &c.down->blocks[curCell->down->idx];
				c.g.cells[block->celly][block->cellx].taken = false;
				c.down->g.cells[block->celly][block->cellx].taken = false;
				c.allocateBlock(waterBlock->id, waterBlock->x, waterBlock->y);
				c.down->allocateBlock(block->id, block->x, block->y);
				block->doKill = true;
				waterBlock->doKill = true;
				return;
			}
			Particle* waterBlock = &c.blocks[curCell->down->idx];
			block->celly++;
			waterBlock->celly--;
			c.updateCellInfo(idx);
			c.updateCellInfo(curCell->down->idx);
		}
	}
}
/*
void updateWood(int idx)
{
	// Wood stays in place
	// Position of current cell
	updateCellInfo(idx);
}

void updateFire(int idx)
{
	// Update color
	c.blocks[idx].color[0] -= 5;
	c.blocks[idx].color[1] -= 5;
	if(c.blocks[idx].color[1] < 50) {
	//	printf("Set doKill\n");
		c.blocks[idx].doKill = true;
		return;
	}
	// Position of current cell
	int posx = c.blocks[idx].cellx;
	int posy = c.blocks[idx].celly;
	updateCellInfo(idx);
	// Calculate possible locations
	int next_row = posy + c.blocks[idx].vel[1];
	if(next_row >= c.g.max_rows)
		next_row = 0;
	int prev_row = posy - c.blocks[idx].vel[1];
	if(prev_row < 0)
		prev_row = 0;
	int next_col = posx + c.blocks[idx].vel[0];
	if(next_col >= c.g.max_cols)
		next_col = posx;
	int prev_col = posx - c.blocks[idx].vel[0];
	if(prev_col < 0)
		prev_col = posx;

	// Falling sand puts fire out
	if(c.g.cells[prev_row][posx].id == SAND && c.g.cells[prev_row][posx].taken) {
		c.blocks[idx].doKill = true;
		return;	
	}

	// Only spread to wood
	// First, left or right
	if(c.g.cells[posy][prev_col].id == WOOD && c.g.cells[posy][prev_col].taken) {
		changeParticle(c.blocks[idx], c.g.cells[posy][prev_col].idx);
		int leftside = prev_col-1 >= 0 ? prev_col-1 : prev_col;
		int rightside = next_col+1 < c.xres ? next_col+1 : next_col;
		// Generate smoke when wood burns
		if(!c.g.cells[prev_row][prev_col].taken) {
			int xpos = c.g.cells[prev_row][prev_col].x;	
			int ypos = c.g.cells[prev_row][prev_col].y;
			allocateParticle(SMOKE, xpos, ypos, prev_col, prev_row);
		}
		if(!c.g.cells[posy][leftside].taken) {
			int xpos = c.g.cells[posy][leftside].x;
			int ypos = c.g.cells[posy][leftside].y;
			allocateParticle(SMOKE, xpos, ypos, leftside, posy);
		}
		if(!c.g.cells[posy][rightside].taken) {
			int xpos = c.g.cells[posy][rightside].x;
			int ypos = c.g.cells[posy][rightside].y;
			allocateParticle(SMOKE, xpos, ypos, rightside, posy);
		}
	}
	if(c.g.cells[posy][next_col].id == WOOD && c.g.cells[posy][next_col].taken) {
		changeParticle(c.blocks[idx], c.g.cells[posy][next_col].idx);
		int leftside = prev_col-1 >= 0 ? prev_col-1 : prev_col;
		int rightside = next_col+1 < c.xres ? next_col+1 : next_col;
		// Generate smoke when wood burns
		if(!c.g.cells[prev_row][prev_col].taken) {
			int xpos = c.g.cells[prev_row][next_col].x;	
			int ypos = c.g.cells[prev_row][next_col].y;
			allocateParticle(SMOKE, xpos, ypos, prev_col, prev_row);
		}
		if(!c.g.cells[posy][leftside].taken) {
			int xpos = c.g.cells[posy][leftside].x;
			int ypos = c.g.cells[posy][leftside].y;
			allocateParticle(SMOKE, xpos, ypos, leftside, posy);
		}
		if(!c.g.cells[posy][rightside].taken) {
			int xpos = c.g.cells[posy][rightside].x;
			int ypos = c.g.cells[posy][rightside].y;
			allocateParticle(SMOKE, xpos, ypos, rightside, posy);
		}
	}
	// Then up or down
	if(c.g.cells[prev_row][posx].id == WOOD && c.g.cells[prev_row][posx].taken) {
		changeParticle(c.blocks[idx], c.g.cells[prev_row][posx].idx);
		int leftside = posx-1 >= 0 ? posx-1 : posx;
		int rightside = posx+1 < c.xres ? posx+1 : posx;
		int above = prev_row-1 >= 0 ? prev_row-1 : prev_row;
		// Generate smoke when wood burns
		if(!c.g.cells[above][posx].taken) {
			int xpos = c.g.cells[above][posx].x;	
			int ypos = c.g.cells[above][posx].y;
			allocateParticle(SMOKE, xpos, ypos, posx, above);
		}
		if(!c.g.cells[prev_row][leftside].taken) {
			int xpos = c.g.cells[prev_row][leftside].x;
			int ypos = c.g.cells[prev_row][leftside].y;
			allocateParticle(SMOKE, xpos, ypos, leftside, prev_row);
		}
		if(!c.g.cells[prev_row][rightside].taken) {
			int xpos = c.g.cells[prev_row][rightside].x;
			int ypos = c.g.cells[prev_row][rightside].y;
			allocateParticle(SMOKE, xpos, ypos, rightside, prev_row);
		}
	}
	if(c.g.cells[next_row][posx].id == WOOD && c.g.cells[next_row][posx].taken) {
		changeParticle(c.blocks[idx], c.g.cells[next_row][posx].idx);
		int leftside = posx-1 >= 0 ? posx-1 : posx;
		int rightside = posx+1 < c.xres ? posx+1 : posx;
		int below = next_row+1 >= 0 ? next_row+1 : next_row;
		// Generate smoke when wood burns
		if(!c.g.cells[below][posx].taken) {
			int xpos = c.g.cells[below][posx].x;	
			int ypos = c.g.cells[below][posx].y;
			allocateParticle(SMOKE, xpos, ypos, posx, below);
		}
		if(!c.g.cells[prev_row][leftside].taken) {
			int xpos = c.g.cells[prev_row][leftside].x;
			int ypos = c.g.cells[prev_row][leftside].y;
			allocateParticle(SMOKE, xpos, ypos, leftside, prev_row);
		}
		if(!c.g.cells[prev_row][rightside].taken) {
			int xpos = c.g.cells[prev_row][rightside].x;
			int ypos = c.g.cells[prev_row][rightside].y;
			allocateParticle(SMOKE, xpos, ypos, rightside, prev_row);
		}
	}
}

void updateSmoke(int idx)
{
	// Update color
	c.blocks[idx].color[0] -= 3;
	c.blocks[idx].color[1] -= 3;
	c.blocks[idx].color[2] -= 3;
	if(c.blocks[idx].color[0] < 25) {
		c.blocks[idx].doKill = true;
		return;
	}
	int posx = c.blocks[idx].cellx;
	int posy = c.blocks[idx].celly;
	updateCellInfo(idx);
	// Calculate possible locations
	int next_row = posy + c.blocks[idx].vel[1];
	if(next_row >= c.g.max_rows)
		next_row = posy;
	int prev_row = posy - c.blocks[idx].vel[1];
	if(prev_row < 0)
		prev_row = posy;
	int next_col = posx + c.blocks[idx].vel[0];
	if(next_col >= c.g.max_cols)
		next_col = posx;
	int prev_col = posx - c.blocks[idx].vel[0];
	if(prev_col < 0)
		prev_col = posx;

	// Always move smoke up
	if(!c.g.cells[prev_row][posx].taken) {
		// Set current cell to not taken & take next spot
		c.g.cells[posy][posx].taken = false;
		c.g.cells[posy][posx].idx = -1;
		c.g.cells[posy][posx].id = -1;
		c.g.cells[prev_row][posx].taken = true;
		// Update current spot
		c.blocks[idx].celly = prev_row;
		posy = c.blocks[idx].celly;
		// Update coords
		c.blocks[idx].x = c.g.cells[posy][posx].x;
		c.blocks[idx].y = c.g.cells[posy][posx].y;
		updateCellInfo(idx);
	} else if(!c.g.cells[prev_row][prev_col].taken) {
		// Set current cell to not taken & take next spot
		c.g.cells[posy][posx].taken = false;
		c.g.cells[posy][posx].idx = -1;
		c.g.cells[posy][posx].id = -1;
		c.g.cells[prev_row][prev_col].taken = true;
		// Update current spot
		c.blocks[idx].celly = prev_row;
		c.blocks[idx].cellx = prev_col;
		posy = c.blocks[idx].celly;
		posx = c.blocks[idx].cellx;
		// Update coords
		c.blocks[idx].x = c.g.cells[posy][posx].x;
		c.blocks[idx].y = c.g.cells[posy][posx].y;
		updateCellInfo(idx);
		return;
	} else if(!c.g.cells[prev_row][next_col].taken) {
		// Set current cell to not taken & take next spot
		c.g.cells[posy][posx].taken = false;
		c.g.cells[posy][posx].idx = -1;
		c.g.cells[posy][posx].id = -1;
		c.g.cells[prev_row][next_col].taken = true;
		// Update current spot
		c.blocks[idx].celly = prev_row;
		c.blocks[idx].cellx = next_col;
		posy = c.blocks[idx].celly;
		posx = c.blocks[idx].cellx;
		// Update coords
		c.blocks[idx].x = c.g.cells[posy][posx].x;
		c.blocks[idx].y = c.g.cells[posy][posx].y;
		updateCellInfo(idx);
		return;
	} else if(!c.g.cells[posy][prev_col].taken) {
		// Smoke flows to the left
		// Set current cell to not taken & take next spot
		c.g.cells[posy][posx].taken = false;
		c.g.cells[posy][posx].idx = -1;
		c.g.cells[posy][posx].id = -1;
		c.g.cells[posy][prev_col].taken = true;
		// Update current spot
		c.blocks[idx].cellx = prev_col;
		posy = c.blocks[idx].celly;
		posx = c.blocks[idx].cellx;
		// Update coords
		c.blocks[idx].x = c.g.cells[posy][posx].x;
		c.blocks[idx].y = c.g.cells[posy][posx].y;
	} else if(!c.g.cells[posy][next_col].taken) {
		// Smoke flows to the right
		// Set current cell to not taken & take next spot
		c.g.cells[posy][posx].taken = false;
		c.g.cells[posy][posx].idx = -1;
		c.g.cells[posy][posx].id = -1;
		c.g.cells[posy][next_col].taken = true;
		// Update current spot
		c.blocks[idx].cellx = next_col;
		posy = c.blocks[idx].celly;
		posx = c.blocks[idx].cellx;
		// Update coords
		c.blocks[idx].x = c.g.cells[posy][posx].x;
		c.blocks[idx].y = c.g.cells[posy][posx].y;
	}
	
	// Create smoke
	static int doMake = 0;
	doMake++;
	if(doMake < 75) return;
	else doMake = 0;
	// Calculate possible locations
	next_row = posy + c.blocks[idx].vel[1];
	if(next_row >= c.g.max_rows)
		next_row = posy;
	prev_row = posy - c.blocks[idx].vel[1];
	if(prev_row < 0)
		prev_row = posy;
	next_col = posx + c.blocks[idx].vel[0];
	if(next_col >= c.g.max_cols)
		next_col = posx;
	prev_col = posx - c.blocks[idx].vel[0];
	if(prev_col < 0)
		prev_col = posx;
	
	if(!c.g.cells[prev_row][prev_col].taken) {
		float xpos = c.g.cells[prev_row][prev_col].x;
		float ypos = c.g.cells[prev_row][prev_col].y;
		allocateParticle(SMOKE, xpos, ypos, prev_col, prev_row);
	} else if(!c.g.cells[prev_row][next_col].taken) {
		float xpos = c.g.cells[prev_row][next_col].x;
		float ypos = c.g.cells[prev_row][next_col].y;
		allocateParticle(SMOKE, xpos, ypos, next_col, prev_row);
	}
}

void updateSteam(int idx)
{
	// Steam moves more slowly
	static int decrease = 0;
	decrease++;
	if(decrease < 2) return;
	decrease = 0;
	c.blocks[idx].color[0]--;
	c.blocks[idx].color[1]--;
	c.blocks[idx].color[2]--;
	if(c.blocks[idx].color[0] < 140) {
		c.blocks[idx].doKill = true;
		return;
	}
	int posx = c.blocks[idx].cellx;
	int posy = c.blocks[idx].celly;
	updateCellInfo(idx);
	// Calculate possible locations
	int next_row = posy + c.blocks[idx].vel[1];
	if(next_row >= c.g.max_rows)
		next_row = posy;
	int prev_row = posy - c.blocks[idx].vel[1];
	if(prev_row < 0)
		prev_row = posy;
	int next_col = posx + c.blocks[idx].vel[0];
	if(next_col >= c.g.max_cols)
		next_col = posx;
	int prev_col = posx - c.blocks[idx].vel[0];
	if(prev_col < 0)
		prev_col = posx;

	// Always move steam up
	if(!c.g.cells[prev_row][posx].taken) {
		// Set current cell to not taken & take next spot
		c.g.cells[posy][posx].taken = false;
		c.g.cells[posy][posx].idx = -1;
		c.g.cells[posy][posx].id = -1;
		c.g.cells[prev_row][posx].taken = true;
		// Update current spot
		c.blocks[idx].celly = prev_row;
		posy = c.blocks[idx].celly;
		// Update coords
		c.blocks[idx].x = c.g.cells[posy][posx].x;
		c.blocks[idx].y = c.g.cells[posy][posx].y;
		updateCellInfo(idx);
		return;
	} else if(!c.g.cells[prev_row][prev_col].taken) {
		// Set current cell to not taken & take next spot
		c.g.cells[posy][posx].taken = false;
		c.g.cells[posy][posx].idx = -1;
		c.g.cells[posy][posx].id = -1;
		c.g.cells[prev_row][prev_col].taken = true;
		// Update current spot
		c.blocks[idx].celly = prev_row;
		c.blocks[idx].cellx = prev_col;
		posy = c.blocks[idx].celly;
		posx = c.blocks[idx].cellx;
		// Update coords
		c.blocks[idx].x = c.g.cells[posy][posx].x;
		c.blocks[idx].y = c.g.cells[posy][posx].y;
		updateCellInfo(idx);
		return;
	} else if(!c.g.cells[prev_row][next_col].taken) {
		// Set current cell to not taken & take next spot
		c.g.cells[posy][posx].taken = false;
		c.g.cells[posy][posx].idx = -1;
		c.g.cells[posy][posx].id = -1;
		c.g.cells[prev_row][next_col].taken = true;
		// Update current spot
		c.blocks[idx].celly = prev_row;
		c.blocks[idx].cellx = next_col;
		posy = c.blocks[idx].celly;
		posx = c.blocks[idx].cellx;
		// Update coords
		c.blocks[idx].x = c.g.cells[posy][posx].x;
		c.blocks[idx].y = c.g.cells[posy][posx].y;
		updateCellInfo(idx);
		return;
	} else if(!c.g.cells[posy][prev_col].taken) {
		// Smoke flows to the left
		// Set current cell to not taken & take next spot
		c.g.cells[posy][posx].taken = false;
		c.g.cells[posy][posx].idx = -1;
		c.g.cells[posy][posx].id = -1;
		c.g.cells[posy][prev_col].taken = true;
		// Update current spot
		c.blocks[idx].cellx = prev_col;
		posy = c.blocks[idx].celly;
		posx = c.blocks[idx].cellx;
		// Update coords
		c.blocks[idx].x = c.g.cells[posy][posx].x;
		c.blocks[idx].y = c.g.cells[posy][posx].y;
	} else if(!c.g.cells[posy][next_col].taken) {
		// Smoke flows to the right
		// Set current cell to not taken & take next spot
		c.g.cells[posy][posx].taken = false;
		c.g.cells[posy][posx].idx = -1;
		c.g.cells[posy][posx].id = -1;
		c.g.cells[posy][next_col].taken = true;
		// Update current spot
		c.blocks[idx].cellx = next_col;
		posy = c.blocks[idx].celly;
		posx = c.blocks[idx].cellx;
		// Update coords
		c.blocks[idx].x = c.g.cells[posy][posx].x;
		c.blocks[idx].y = c.g.cells[posy][posx].y;
	}

	// If nothing around, create more steam
	posx = c.blocks[idx].cellx;
	posy = c.blocks[idx].celly;
	updateCellInfo(idx);
	// Calculate possible locations
	next_row = posy + c.blocks[idx].vel[1];
	if(next_row >= c.g.max_rows)
		next_row = posy;
	prev_row = posy - c.blocks[idx].vel[1];
	if(prev_row < 0)
		prev_row = posy;
	next_col = posx + c.blocks[idx].vel[0];
	if(next_col >= c.g.max_cols)
		next_col = posx;
	prev_col = posx - c.blocks[idx].vel[0];
	if(prev_col < 0)
		prev_col = posx;
	
	// Create steam
	if(!c.g.cells[prev_row][posx].taken) {
		float xpos = c.g.cells[prev_row][posx].x;
		float ypos = c.g.cells[prev_row][posx].y;
		allocateParticle(STEAM, xpos, ypos, posx, prev_row);
	} if(!c.g.cells[prev_row][prev_col].taken) {
		float xpos = c.g.cells[prev_row][prev_col].x;
		float ypos = c.g.cells[prev_row][prev_col].y;
		allocateParticle(STEAM, xpos, ypos, prev_col, prev_row);
	} if(!c.g.cells[prev_row][next_col].taken) {
		float xpos = c.g.cells[prev_row][next_col].x;
		float ypos = c.g.cells[prev_row][next_col].y;
		allocateParticle(STEAM, xpos, ypos, next_col, prev_row);
	}
}

void updateSalt(int idx)
{
	const short int ACC = c.blocks[idx].Tacc;
	if(c.blocks[idx].color[0] < 255) {
		c.blocks[idx].doKill = true;
		return;
	}
	bool do_return = false;
	// Position of current cell
	int posx = c.blocks[idx].cellx;
	int posy = c.blocks[idx].celly;
	updateCellInfo(idx);
	// Calculate possible locations
	int next_row = posy + c.blocks[idx].vel[1];
	if(next_row >= c.g.max_rows)
		return;
	int prev_row = posy - c.blocks[idx].vel[1];
	if(prev_row < 0)
		prev_row = posy;
	int next_col = posx + c.blocks[idx].vel[0];
	if(next_col >= c.g.max_cols)
		next_col = posx;
	int prev_col = posx - c.blocks[idx].vel[0];
	if(prev_col < 0)
		prev_col = posx;

	// Salt evaporates with water
	if(c.g.cells[prev_row][posx].id == WATER) {
		// Above
		c.blocks[idx].color[0] -= 5;
	} else if(c.g.cells[next_row][posx].id == WATER && c.g.cells[next_row][posx].taken) {
		// Below
		c.blocks[idx].color[0] -= 5;
	} else if(c.g.cells[posy][next_col].id == WATER && c.g.cells[posy][next_col].taken) {
		// Right
		c.blocks[idx].color[0] -= 5;
	} else if(c.g.cells[posy][prev_col].id == WATER && c.g.cells[posy][prev_col].taken) {
		// Left
		c.blocks[idx].color[0] -= 5;
	}
	// Update logic for salt
	if(!c.g.cells[next_row][posx].taken) {
		// Salt falls directly down
		if(c.blocks[idx].acc < ACC)
			c.blocks[idx].acc++;
		int dist = 0;
		while(!c.g.cells[next_row][posx].taken && dist < c.blocks[idx].acc) {
			// Set current cell to not taken & take next spot
			c.g.cells[posy][posx].taken = false;
			c.g.cells[posy][posx].idx = -1;
			c.g.cells[posy][posx].id = -1;
			c.g.cells[next_row][posx].taken = true;
			// Update current spot
			c.blocks[idx].celly = next_row;
			posy = c.blocks[idx].celly;
			// Update coords
			c.blocks[idx].x = c.g.cells[posy][posx].x;
			c.blocks[idx].y = c.g.cells[posy][posx].y;
			// Calculate possible locations
			next_row = posy + c.blocks[idx].vel[1];
			if(next_row >= c.g.max_rows)
				return;
			next_col = posx + c.blocks[idx].vel[0];
			if(next_col >= c.g.max_cols)
				next_col = posx;
			prev_col = posx - c.blocks[idx].vel[0];
			if(prev_col < 0)
				prev_col = posx;
			dist++;
		}
		return;
	} else if(!c.g.cells[next_row][next_col].taken) {
		// Salt falls down->right
		if(c.blocks[idx].acc < ACC)
			c.blocks[idx].acc++;
		int dist = 0;
		while(!c.g.cells[next_row][next_col].taken && dist < c.blocks[idx].acc) {
			// Set current cell to not taken & take next spot
			c.g.cells[posy][posx].taken = false;
			c.g.cells[posy][posx].idx = -1;
			c.g.cells[posy][posx].id = -1;
			c.g.cells[next_row][next_col].taken = true;
			// Update current spot
			c.blocks[idx].celly = next_row;
			c.blocks[idx].cellx = next_col;
			posy = c.blocks[idx].celly;
			posx = c.blocks[idx].cellx;
			// Update coords
			c.blocks[idx].x = c.g.cells[posy][posx].x;
			c.blocks[idx].y = c.g.cells[posy][posx].y;
			// Calculate possible locations
			next_row = posy + c.blocks[idx].vel[1];
			if(next_row >= c.g.max_rows)
				return;
			next_col = posx + c.blocks[idx].vel[0];
			if(next_col >= c.g.max_cols)
				next_col = posx;
			prev_col = posx - c.blocks[idx].vel[0];
			if(prev_col < 0)
				prev_col = posx;
			dist++;
		}
		return;
	} else if(!c.g.cells[next_row][prev_col].taken) {
		// Salt falls down->left
		if(c.blocks[idx].acc < ACC)
			c.blocks[idx].acc++;
		int dist = 0;
		while(!c.g.cells[next_row][prev_col].taken && dist < c.blocks[idx].acc) {
			// Set current cell to not taken & take next spot
			c.g.cells[posy][posx].taken = false;
			c.g.cells[posy][posx].idx = -1;
			c.g.cells[posy][posx].id = -1;
			c.g.cells[next_row][prev_col].taken = true;
			// Update current spot
			c.blocks[idx].celly = next_row;
			c.blocks[idx].cellx = prev_col;
			posy = c.blocks[idx].celly;
			posx = c.blocks[idx].cellx;
			// Update coords
			c.blocks[idx].x = c.g.cells[posy][posx].x;
			c.blocks[idx].y = c.g.cells[posy][posx].y;
			// Calculate possible locations
			next_row = posy + c.blocks[idx].vel[1];
			if(next_row >= c.g.max_rows)
				return;
			next_col = posx + c.blocks[idx].vel[0];
			if(next_col >= c.g.max_cols)
				next_col = posx;
			prev_col = posx - c.blocks[idx].vel[0];
			if(prev_col < 0)
				prev_col = posx;
			dist++;
		}
		return;
	} else {
		// This object has stopped
		// Restart acc
		c.blocks[idx].acc = 0;
	}
}

void updateMetal(int idx)
{
	// Stays in place, but interacts with fire & water
	const short int MAX_HEAT = 240;
	const short int MIN_HEAT = 180;
	// Position of current cell
	int posx = c.blocks[idx].cellx;
	int posy = c.blocks[idx].celly;
	updateCellInfo(idx);
	// Calculate possible locations
	int next_row = posy + c.blocks[idx].vel[1];
	if(next_row >= c.g.max_rows)
		return;
	int prev_row = posy - c.blocks[idx].vel[1];
	if(prev_row < 0)
		prev_row = posy;
	int next_col = posx + c.blocks[idx].vel[0];
	if(next_col >= c.g.max_cols)
		next_col = posx;
	int prev_col = posx - c.blocks[idx].vel[0];
	if(prev_col < 0)
		prev_col = posx;

	// Fire interactions heat up metal
	if(c.g.cells[prev_row][posx].id == FIRE && c.g.cells[prev_row][posx].taken) {
		if(c.blocks[idx].color[0] < MAX_HEAT)
			c.blocks[idx].color[0] += 5;
	} else if(c.g.cells[next_row][posx].id == FIRE && c.g.cells[next_row][posx].taken) {
		if(c.blocks[idx].color[0] < MAX_HEAT)
			c.blocks[idx].color[0] += 5;
	} else if(c.g.cells[posy][next_col].id == FIRE && c.g.cells[posy][next_col].taken) {
		if(c.blocks[idx].color[0] < MAX_HEAT)
			c.blocks[idx].color[0] += 5;
	} else if(c.g.cells[posy][prev_col].id == FIRE && c.g.cells[posy][prev_col].taken) {
		if(c.blocks[idx].color[0] < MAX_HEAT)
			c.blocks[idx].color[0] += 5;
	}

	// Water interactions cool metal
	if(c.g.cells[prev_row][posx].id == WATER && c.g.cells[prev_row][posx].taken) {
		if(c.blocks[idx].color[0] > MIN_HEAT) {
			c.blocks[idx].color[0] -= 5;
			int idx1 = c.g.cells[prev_row][posx].idx;
			c.blocks[idx1].id = STEAM;
			c.blocks[idx1].reinit();
		}
	} else if(c.g.cells[next_row][posx].id == WATER && c.g.cells[next_row][posx].taken) {
		if(c.blocks[idx].color[0] > MIN_HEAT) {
			c.blocks[idx].color[0] -= 5;
			int idx1 = c.g.cells[next_row][posx].idx;
			c.blocks[idx1].id = STEAM;
			c.blocks[idx1].reinit();
		}
	} else if(c.g.cells[posy][next_col].id == WATER && c.g.cells[posy][next_col].taken) {
		if(c.blocks[idx].color[0] > MIN_HEAT) {
			c.blocks[idx].color[0] -= 5;
			int idx1 = c.g.cells[posy][next_col].idx;
			c.blocks[idx1].id = STEAM;
			c.blocks[idx1].reinit();
		}
	} else if(c.g.cells[posy][prev_col].id == WATER && c.g.cells[posy][prev_col].taken) {
		if(c.blocks[idx].color[0] > MIN_HEAT) {
			c.blocks[idx].color[0] -= 5;
			int idx1 = c.g.cells[posy][prev_col].idx;
			c.blocks[idx1].id = STEAM;
			c.blocks[idx1].reinit();
		}
	}
}

*/

// End ********************
