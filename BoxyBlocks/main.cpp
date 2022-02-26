#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <chrono>
#include <string>
#include "game.h"

#define main SDL_main    //Original line. Renames main(){} to SDL_main(){}. 

#define main main        //Added line. Undo the renaming.

void physics()
{

}

void mainMenu()
{

}

void debugDrawChunks()
{

}

void render()
{

}

int main(int argc, char** argv)
{
	Game game;
	//srand(time(NULL));
	// SDL
	if (game.Init())
		return 0;
	bool quit = false;
	
	// Calculating FPS
	int frame_count = 0;
	auto one_sec = std::chrono::high_resolution_clock::now();
	auto clock = std::chrono::high_resolution_clock::now();

	while(!quit) {
		frame_count++;
		// All input
		quit = game.HandleEvents();
		// Main program display
		
		// Continually generate/delete particles if mouse button is held
		if(game.lbutton_down && !game.show_menu) {
			//game.chunks[game.curChunkY][game.curChunkX].allocateBlock(game.select, game.mousex, game.mousey);
		}
		if(game.rbutton_down) {
			float mousePos[2] = {(float)game.mousex, (float)game.mousey};
            //game.chunks[game.curChunkY][game.curChunkX].deallocateBlocks(mousePos);
		}

		game.Render();
		// FPS count
		//auto now = std::chrono::high_resolution_clock::now();
		//std::chrono::duration<float, std::milli> update = now-one_sec;

		//if(update.count() >= 1000) {
		//	// Restart the sec
		//	one_sec = std::chrono::high_resolution_clock::now();
		//	
		//	now = std::chrono::high_resolution_clock::now();
		//	std::chrono::duration<float, std::milli> elapsed = now-clock;
		//	
		//	//printf("\33[2K\r");
		//	//printf("\rFPS: %.2f, Show Menu: %i, Total Chunks: %i, Rendered Chunks: %i, Mouse Position: %i, %i, Camera Focus: %.2f, %.2f, Mouse in chunk %i %i",\
		//	// 	frame_count / (elapsed.count() / 1000.0f), game.show_menu, CHUNKSX*CHUNKSY, game.rendChunks, game.mousex, game.mousey, game.cam.focusx,\
		//	//	game.cam.focusy, game.curChunkY, game.curChunkX);
		//	//fflush(stdout);
		//}
	}

	return 0;
}

