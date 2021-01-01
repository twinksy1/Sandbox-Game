#include "game.h"

Camera::Camera()
{
    focusx = (float)xres / 2.0f;
    focusy = (float)yres / 2.0f;
}

Game::Game()
{
    rendChunks = 0;
    // Which type of particle to generate
    select = 0;
    mousex = mousey = 0;
    // Initialize chunks
    int startChunkNo = 1001;
    int chunkInc = 1;
    float x = 0;
    float y = 0;
    for(int i=0; i<CHUNKSY; i++) {
        x = 0;
        for(int j=0; j<CHUNKSX; j++) {
            chunks[i][j].initChunk(startChunkNo, x, y);
            float xpos = chunks[i][j].startX;
            float ypos = chunks[i][j].startY;
            
            float corner1[2] = {xpos, ypos};
            float corner2[2] = {xpos, ypos + MAX_CHUNK_SIZE};
            float corner3[2] = {xpos + MAX_CHUNK_SIZE, ypos};
            float corner4[2] = {xpos + MAX_CHUNK_SIZE, ypos + MAX_CHUNK_SIZE};

            float camPos[2] = {cam.focusx, cam.focusy};
            if(getDist(camPos, corner1) < CHUNK_REND_DIST) {
                chunks[i][j].doRender = true;
            } else if(getDist(camPos, corner2) < CHUNK_REND_DIST) {
                chunks[i][j].doRender = true;
            } else if(getDist(camPos, corner3) < CHUNK_REND_DIST) {
                chunks[i][j].doRender = true;
            } else if(getDist(camPos, corner4) < CHUNK_REND_DIST) {
                chunks[i][j].doRender = true;
            } else {
                chunks[i][j].doRender = false;
            }

            startChunkNo += chunkInc;
            x += MAX_CHUNK_SIZE;
        }
        y += MAX_CHUNK_SIZE;
    }
    // Stitch chunks together
    Chunk* cur;
    Chunk* prev;
    Chunk* next;
    Chunk* top;
    Chunk* bottom;
    int gridRows = chunks[0][0].g.max_rows;
	int gridCols = chunks[0][0].g.max_cols;

    for(int i=0; i<CHUNKSY; i++) {
        for(int j=0; j<CHUNKSX; j++) {
            if(j > 0) {
                cur = &chunks[i][j];
                prev = &chunks[i][j-1];
                cur->left = prev;
                for(int k=0; k<gridRows; k++) {
                    cur->g.cells[k][0].left = &prev->g.cells[k][gridCols-1];
                }
            }
            if(j < CHUNKSX-1) {
                cur = &chunks[i][j];
                next = &chunks[i][j+1];
                cur->right = next;
                for(int k=0; k<gridRows; k++) {
                    cur->g.cells[k][gridCols-1].right = &next->g.cells[k][0];
                }
            }
            if(i < CHUNKSY-1) {
                cur = &chunks[i][j];
                bottom = &chunks[i+1][j];
                cur->down = bottom;
                for(int k=0; k<gridCols; k++) {
                    cur->g.cells[gridRows-1][k].down = &bottom->g.cells[0][k];
                }
            }
            if(i > 0) {
                cur = &chunks[i][j];
                top = &chunks[i-1][j];
                cur->up = top;
                for(int k=0; k<gridCols; k++) {
                    cur->g.cells[0][k].up = &top->g.cells[gridRows-1][k];
                }
            }
        }
    }
}
Game::~Game()
{
}

void Game::displayCellInfo(float* pos)
{
    Chunk* cur = &chunks[curChunkY][curChunkX];
    int idxX, idxY;
    float minDist = 9999.9f;
    int rows = cur->g.max_rows;
    int cols = cur->g.max_cols;
    for(int i=0; i<rows; i++) {
        for(int j=0; j<cols; j++) {
            float cellPos[2] = {(float)cur->g.cells[i][j].x, 
                                (float)cur->g.cells[i][j].y };
            float dist = getDist(cellPos, pos);
            if(dist < minDist) {
                minDist = dist;
                idxX = j;
                idxY = i;
            }
        }
    }

    printf("\nCHUNK #%i: [%i][%i]\n", cur->chunkID, idxY, idxX);
    printf("IDX: %i, ID: %i, TAKEN: %i, Instances: %i\n", cur->g.cells[idxY][idxX].idx,\
            cur->g.cells[idxY][idxX].id, cur->g.cells[idxY][idxX].taken, cur->g.cells[idxY][idxX].instances);

    for(int i=0; i<cur->blockCount; i++) {
        if(cur->blocks[i].cellx == idxX && cur->blocks[i].celly == idxY) {
            printf("\tPARTICLE: idx: %i, id: %i\n", cur->blocks[i].idx, cur->blocks[i].id);
        }
    }
}

void Game::pan(int dir)
{
    if(dir == -1) {
        // Left
        if(chunks[CHUNKSY-1][0].startX - cam.panSpeed > 0) return;
        for(int i=0; i<CHUNKSY; i++) {
            for(int j=0; j<CHUNKSX; j++) {
                chunks[i][j].startX -= cam.panSpeed;
                float xpos = chunks[i][j].startX;
                float ypos = chunks[i][j].startY;

                float corner1[2] = {xpos, ypos};
                float corner2[2] = {xpos, ypos + MAX_CHUNK_SIZE};
                float corner3[2] = {xpos + MAX_CHUNK_SIZE, ypos};
                float corner4[2] = {xpos + MAX_CHUNK_SIZE, ypos + MAX_CHUNK_SIZE};

                float camPos[2] = {cam.focusx, cam.focusy};
                if(getDist(camPos, corner1) < CHUNK_REND_DIST) {
                    chunks[i][j].doRender = true;
                    chunks[i][j].loadChunk();
                } else if(getDist(camPos, corner2) < CHUNK_REND_DIST) {
                    chunks[i][j].doRender = true;
                    chunks[i][j].loadChunk();
                } else if(getDist(camPos, corner3) < CHUNK_REND_DIST) {
                    chunks[i][j].doRender = true;
                    chunks[i][j].loadChunk();
                } else if(getDist(camPos, corner4) < CHUNK_REND_DIST) {
                    chunks[i][j].doRender = true;
                    chunks[i][j].loadChunk();
                } else {
                    chunks[i][j].doRender = false;
                }
            }
        }
    } else {
        // Right
        if(chunks[0][0].startX + cam.panSpeed > 0) return;
        for(int i=0; i<CHUNKSY; i++) {
            for(int j=0; j<CHUNKSX; j++) {
                chunks[i][j].startX += cam.panSpeed;
                float xpos = chunks[i][j].startX;
                float ypos = chunks[i][j].startY;

                float corner1[2] = {xpos, ypos};
                float corner2[2] = {xpos, ypos + MAX_CHUNK_SIZE};
                float corner3[2] = {xpos + MAX_CHUNK_SIZE, ypos};
                float corner4[2] = {xpos + MAX_CHUNK_SIZE, ypos + MAX_CHUNK_SIZE};

                float camPos[2] = {cam.focusx, cam.focusy};
                if(getDist(camPos, corner1) < CHUNK_REND_DIST) {
                    chunks[i][j].doRender = true;
                    chunks[i][j].loadChunk();
                } else if(getDist(camPos, corner2) < CHUNK_REND_DIST) {
                    chunks[i][j].doRender = true;
                    chunks[i][j].loadChunk();
                } else if(getDist(camPos, corner3) < CHUNK_REND_DIST) {
                    chunks[i][j].doRender = true;
                    chunks[i][j].loadChunk();
                } else if(getDist(camPos, corner4) < CHUNK_REND_DIST) {
                    chunks[i][j].doRender = true;
                    chunks[i][j].loadChunk();
                } else {
                    chunks[i][j].doRender = false;
                }
            }
        }
    }
}