#include<cstdio>
#include<cstdlib>
#include<cmath>
#include<chrono>
#include<string>
#include<unistd.h>

#include "display.h"

Display s;
Game game;


/*
void deleteParticles()
{
	game.lbutton_down = false;
	game.rbutton_down = true;
	float mouse[2] = {(float)game.mousex, (float)game.mousey};
	
	for(int i=0; i<game.pcount; i++) {
		float pos[2] = {game.p[i].x, game.p[i].y};
		float dist = getLength(pos, mouse);
		if(dist < game.radius)
			game.p[i].doKill = true;
	}

	deallocateParticles();
}

void generateParticles()
{
	game.lbutton_down = true;
	game.rbutton_down = false;
	if(game.show_menu && game.mousey < 100) return;
	if(!game.inbounds) return;
	float mouse[2] = {(float)game.mousex, (float)game.mousey};
	float closest_dist = 999.99f;
	int celly_idx;
	int cellx_idx;

	for(int i=0; i<g.max_rows; i++) {
		for(int j=0; j<g.max_cols; j++) {
			float cell_pos[2] = {(float)g.cells[i][j].x, 
				(float)g.cells[i][j].y};
			float dist = getLength(mouse, cell_pos);
			if(dist < closest_dist && !g.cells[i][j].taken) {
				closest_dist = dist;
				celly_idx = i;
				cellx_idx = j;
			}
		}
	}
	
	// Generate a new particle
	if(closest_dist < game.radius) {
		allocateParticle(game.select,
			       g.cells[celly_idx][cellx_idx].x,
		       	   g.cells[celly_idx][cellx_idx].y,
				   cellx_idx, celly_idx);
	}

}
*/

void physics()
{
	/*
	static auto time_since_burn = std::chrono::high_resolution_clock::now();
	
	// Delete particles with the doKill set true
	deallocateParticles();
	*/
}

void mainMenu()
{
	// Displays menu which allows choosing between blocks
	s.setColor(0, 200, 150);
	s.fillRect(0, 0, xres, 100);
	int x = 0, y = 0, w = 20, h = 20;
	int offsetx = 25, offsety = 25;
	int textSize = 25;


	for(int i=0; i<NUM_PARTICLES; i++) {
		if(game.select == i) {
			// Highlight current option
			s.setColor(0, 240, 150);
			s.fillRect(x, y, (2*offsetx), (2*offsety));
		}
		if(game.mousex >= x && game.mousex < x+(2*offsetx) &&
				game.mousey >= y && game.mousey < 100) {
			// Highlight new option
			s.setColor(0, 240, 150);
			s.fillRect(x, y, (2*offsetx), (2*offsety));
			if(game.lbutton_down) {
				// Choose this particle
				game.select = i;
			}
		}
		s.drawTexture(x+offsetx, y+offsety, i);
		s.drawText(x, y+(offsety/2), w, h, BLOCK_TYPES[i].c_str(), textSize, i);
		x += 2*offsetx;
	}
}

void debugDrawChunks()
{
	Chunk* cur;
	
	for(int i=0; i<CHUNKSY; i++) {
		for(int j=0; j<CHUNKSX; j++) {
			cur = &game.chunks[i][j];
			if(cur->doRender) {
				// Draw grid
				s.setColor(255, 255, 255);
				for(int k=0; k<cur->g.max_rows; k++) {
					for(int j=0; j<cur->g.max_cols; j++) {
						s.drawRect(cur->g.cells[k][j].x, cur->g.cells[k][j].y,
								GRIDSIZEX, GRIDSIZEY);
					}
				}
				s.setColor(255, 0, 0);
				s.drawRect(cur->startX, cur->startY, MAX_CHUNK_SIZE, MAX_CHUNK_SIZE);
			}
		}
	}
}

void render()
{
	int startx = game.chunks[0][0].startX;
	int starty = game.chunks[0][0].startY;
	int w = game.chunks[0][CHUNKSX-1].startX;
	int h = yres;
	s.drawBackground(startx, starty, w, h);


	s.updateTextures();
	if(game.debugMode) {
		debugDrawChunks();
	}

	bool found = false;
	Chunk* cur;
	int count = 0;
	for(int i=0; i<CHUNKSY; i++) {
		for(int j=0; j<CHUNKSX; j++) {
			cur = &game.chunks[i][j];
			if(cur->doRender) {
				count++;
				cur->blockPhysics();
				if(!found) {
					if(cur->within(game.mousex, game.mousey)) {
						found = true;
						game.curChunkX = j;
						game.curChunkY = i;
					}
				}
				for(int k=0; k<cur->blockCount; k++) {
					s.drawTexture(cur->blocks[k].x, cur->blocks[k].y, cur->blocks[k].id);
				}
			}
		}
	}
	
	game.rendChunks = count;

	// Menu overlaps particles
	if(game.show_menu)
		mainMenu();
}

int main(int argc, char** argv)
{
	srand(time(NULL));
	// SDL
	if(s.init())
		exit(EXIT_FAILURE);
	bool quit = false;
	
	// Calculating FPS
	int frame_count = 0;
	auto one_sec = std::chrono::high_resolution_clock::now();
	auto clock = std::chrono::high_resolution_clock::now();

	while(!quit) {
		frame_count++;
		// All input
		quit = s.handleInput(game);
		// Main program display
		
		// Continually generate/delete particles if mouse button is held
		if(game.lbutton_down && !game.show_menu) {
			game.chunks[game.curChunkY][game.curChunkX].allocateBlock(game.select, game.mousex, game.mousey);
		}
		if(game.rbutton_down) {
			float mousePos[2] = {(float)game.mousex, (float)game.mousey};
            game.chunks[game.curChunkY][game.curChunkX].deallocateBlocks(mousePos);
		}

		s.preRender();
		render();
		s.postRender();
		// FPS count
		auto now = std::chrono::high_resolution_clock::now();
		std::chrono::duration<float, std::milli> update = now-one_sec;

		if(update.count() >= 1000) {
			// Restart the sec
			one_sec = std::chrono::high_resolution_clock::now();
			
			now = std::chrono::high_resolution_clock::now();
			std::chrono::duration<float, std::milli> elapsed = now-clock;
			
			printf("\33[2K\r");
			printf("\rFPS: %.2f, Show Menu: %i, Total Chunks: %i, Rendered Chunks: %i, Mouse Position: %i, %i, Camera Focus: %.2f, %.2f, Mouse in chunk %i %i",\
			 	frame_count / (elapsed.count() / 1000.0f), game.show_menu, CHUNKSX*CHUNKSY, game.rendChunks, game.mousex, game.mousey, game.cam.focusx,\
				game.cam.focusy, game.curChunkY, game.curChunkX);
			fflush(stdout);
		}
	}

	return 0;
}

