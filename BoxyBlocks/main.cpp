#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <chrono>
#include <string>
#include "game.h"

#define main SDL_main    //Original line. Renames main(){} to SDL_main(){}. 
#define main main        //Added line. Undo the renaming.

int main(int argc, char** argv)
{
	Game game;
	game.Init();
	if (game.error != Errors::NONE)
		return 0;
	
	//// Calculating FPS
	//int frame_count = 0;
	//auto one_sec = std::chrono::high_resolution_clock::now();
	//auto clock = std::chrono::high_resolution_clock::now();

	while(game.Run()) {
	}

	return 0;
}

