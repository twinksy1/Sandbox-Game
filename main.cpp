#include<cstdio>
#include<cstdlib>
#include<cmath>
#include<chrono>
#include<string>
#include<unistd.h>

#include<SDL2/SDL.h>
#include<SDL2/SDL_ttf.h>

#include "blocks.h"

// Global
class Global {
	public:
	int xres;
	int yres;
	Particle* p;
	int pcount;
	short int select;
	float radius = 10.0f;
	bool flush = false;
	bool shift_down = false;
	bool lbutton_down = false;
	bool rbutton_down = false;
	int mousex, mousey;
	bool show_menu = true;
	bool pause = false;
	bool inbounds = false;
	
	Global()
	{
		xres = 1600;
		yres = 1600;
		p = nullptr;
		pcount = 0;
		// Which type of particle to generate
		select = 0;
		mousex = mousey = 0;
	}
	~Global()
	{
		if(p != nullptr) {
			delete p;
			p = nullptr;
		}
	}
} global;

Grid g;

void allocateParticle(int id, int x, int y, int cellx, int celly)
{
	if(global.pcount < 1) {
		try {
			global.p = new Particle;
			global.p->initParticle(x, y, id, cellx, celly, global.pcount);
			g.cells[celly][cellx].idx = global.p->idx;
			g.cells[celly][cellx].id = global.p->id;
			g.cells[celly][cellx].taken = true;
			global.pcount = 1;
		} catch(int BAD_ALLOC) {
			printf("ERROR ALLOCATING PARTICLE\n");
		}
	} else {
		try {
			Particle* tmp = global.p;
			global.p = new Particle[global.pcount+1];
			for(int i=0; i<global.pcount; i++)
				global.p[i] = tmp[i];
			global.p[global.pcount].initParticle(x, y, id, cellx, celly, global.pcount);
			g.cells[celly][cellx].taken = true;
			g.cells[celly][cellx].idx = global.p[global.pcount].idx;
			g.cells[celly][cellx].id = global.p[global.pcount].id;
			delete tmp;
			tmp = nullptr;
			global.pcount++;
		} catch(int BAD_ALLOC) {
			printf("ERROR ALLOCATING PARTICLE\n");
		}
	}
}

void deallocateParticles()
{
	int kill_count = 0;
	for(int i=0; i<global.pcount; i++)
		if(global.p[i].doKill) {
			kill_count++;
			int xpos = global.p[i].cellx;
			int ypos = global.p[i].celly;
			g.cells[ypos][xpos].taken = false;
			g.cells[ypos][xpos].idx = -1;
			g.cells[ypos][xpos].id = -1;
		}
	if(kill_count == 0) return;
	else if(global.pcount - kill_count <= 0) {
		try {
			delete global.p;
			global.p = nullptr;
			global.pcount = 0;
		} catch(int err) {
			printf("ERROR DEALLOCATING PARTICLES\n");
		}
	} else {
		try {
			Particle* tmp = global.p;
			int new_count = global.pcount - kill_count;
			global.p = new Particle[new_count];
			int j=0;
			for(int i=0; i<global.pcount; i++) {
				if(!tmp[i].doKill && j < new_count) {
					// Copy if no kill order
					global.p[j] = tmp[i];
					j++;
				}
			}

			delete tmp;
			tmp = nullptr;
			global.pcount = new_count;
		} catch(int err) {
			printf("ERROR DEALLOCATING PARTICLES\n");
		}
	}
}

class SDL_Wrapper {
	private:
	SDL_Window* window;
	SDL_Surface* screen_surface;
	SDL_Renderer* rend;
	SDL_Texture* message[NUM_PARTICLES];
	SDL_Event e;
	public:

	SDL_Wrapper()
	{
		window = NULL;
		screen_surface = NULL;
		rend = NULL;
		for(int i=0; i<NUM_PARTICLES; i++)
			message[i] = NULL;
	}
	~SDL_Wrapper()
	{
		SDL_FreeSurface(screen_surface);
		for(int i=0; i<NUM_PARTICLES; i++)
			SDL_DestroyTexture(message[i]);
		SDL_DestroyWindow(window);
		SDL_Quit();
		printf("Successful exit\n");
	}
	int init()
	{
		if(SDL_Init(SDL_INIT_VIDEO) == -1) {
			printf("ERROR SDL_INIT: %s\n", SDL_GetError());
			return 1;
		}

		window = SDL_CreateWindow("Sandbox Simulation", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
				global.xres, global.yres, SDL_WINDOW_SHOWN);
		if(window == NULL) {
			printf("ERROR DISPLAYING WINDOW: %s\n", SDL_GetError());
			return 1;
		}

		rend = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
		if(rend == NULL) {
			printf("ERROR RENDERER: %s\n", SDL_GetError());
			return 1;
		}
		TTF_Init();
		
		return 0;
	}
	void setColor(int r, int g, int b)
	{
		SDL_SetRenderDrawColor(rend, r, g, b, 255);
	}
	void preRender()
	{
		// Rendering function
		//screen_surface = SDL_GetWindowSurface(window);
		//SDL_FillRect(screen_surface, NULL, SDL_MapRGB(screen_surface->format, WHITE[0], WHITE[1], WHITE[2]));
		//SDL_UpdateWindowSurface(window);
	
		// Clear screen
		setColor(0, 0, 0);
		SDL_RenderClear(rend);
	}
	void postRender()
	{
		// Show renderer
		SDL_RenderPresent(rend);
	}
	bool keyboardInput()
	{
		// Key up
		if(e.type == SDL_KEYUP) {
			switch(e.key.keysym.sym) {
				case SDLK_LSHIFT:
					global.shift_down = false;
					//printf("Shift key released\n");
					break;
			}
		}
		// Handle keyboard input
		switch(e.key.keysym.sym) {
			case SDLK_0:
				global.select = SAND;
				break;
			case SDLK_1:
				global.select = WATER;
				break;
			case SDLK_2:
				global.select = WOOD;
				break;
			case SDLK_3:
				global.select = FIRE;
				break;
			case SDLK_4:
				global.select = SMOKE;
				break;
			case SDLK_5:
				global.select = STEAM;
				break;
			case SDLK_m: {
				static bool ignore = false;
				if(!ignore)
					global.show_menu ^= 1;
				ignore ^= 1;
				break;
				     }
			case SDLK_p: {
				static bool ignore = false;
				if(!ignore)
					global.pause ^= 1;
				ignore ^= 1;
				break;
				     }
			case SDLK_f: {
				static bool ignore = false;
				if(!ignore)
					global.flush ^= 1;
				ignore ^= 1;
				break;
				     }
			case SDLK_LSHIFT: {
				static bool ignore = false;
				if(!ignore)
					global.shift_down = true;
				ignore ^= 1;
				break;
					  }
			case SDLK_ESCAPE:
				return true;
				break;
		}
		return false;
	}
	void checkMouse(SDL_Event e)
	{
		if(e.type == SDL_MOUSEMOTION) {
			// Mouse moved
			SDL_GetMouseState(&global.mousex, &global.mousey);
			if(global.mousex >=0 && global.mousex < global.xres && 
					global.mousey >= 0 && global.mousey < global.yres)
				global.inbounds = true;
			else global.inbounds = false;
			//printf("Mouse: %i, %i\n", mousex, mousey);
		}

		if(e.type == SDL_MOUSEBUTTONDOWN) {
			// Mouse click
			// If shift down, display cell info
			if((e.button.button == SDL_BUTTON_LEFT) && global.shift_down) {
				float mouse[2] = {(float)global.mousex, (float)global.mousey};
				float closest_dist = 999.99f;
				// Chosen cell
				int cellx, celly;

				for(int i=0; i<g.max_rows; i++) {
					for(int j=0; j<g.max_cols; j++) {
						float cell_pos[2] = {(float)g.cells[i][j].x,
							(float)g.cells[i][j].y};
						float dist = getLength(mouse, cell_pos);
						if(dist < closest_dist) {
							closest_dist = dist;
							cellx = j;
							celly = i;
						}
					}
				}

				// Print info for this cell
				printf("\n\nCell [%i][%i]\n============\n", celly, cellx);
				printf("ID: %i, Particle IDX: %i, Taken: %i\n",
						g.cells[celly][cellx].id, g.cells[celly][cellx].idx,
						g.cells[celly][cellx].id);
				global.p[g.cells[celly][cellx].idx].printInfo();
			}

			// Else generate a particle
			else if((e.button.button == SDL_BUTTON_LEFT || global.lbutton_down) && global.inbounds) {
				global.lbutton_down = true;
				global.rbutton_down = false;
				if(global.show_menu && global.mousey < 100) return;
				// Left click, generate particle at closest cell
				float mouse[2] = {(float)global.mousex, (float)global.mousey};
				float closest_dist = 999.99f;
				// Index of cell
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
				if(closest_dist < global.radius) {
					allocateParticle(global.select,
						       g.cells[celly_idx][cellx_idx].x,
					       g.cells[celly_idx][cellx_idx].y,	cellx_idx, celly_idx);
				}
			}
			else if((e.button.button == SDL_BUTTON_RIGHT || global.rbutton_down) && global.inbounds) {
				// Right click
				global.lbutton_down = false;
				global.rbutton_down = true;
				float mouse[2] = {(float)global.mousex, (float)global.mousey};
				
				for(int i=0; i<global.pcount; i++) {
					float pos[2] = {global.p[i].x, global.p[i].y};
					float dist = getLength(pos, mouse);
					if(dist < global.radius)
						global.p[i].doKill = true;
				}

				deallocateParticles();
			}
		}

		if(e.type == SDL_MOUSEBUTTONUP) {
			// Button release
			global.lbutton_down = false;
			global.rbutton_down = false;
		}
	}
	bool handleInput()
	{
		// Handles all events in queue
		while(SDL_PollEvent(&e) != 0) {
			if(e.type == SDL_QUIT)
				return true;
			else if(e.type == SDL_KEYDOWN || e.type == SDL_KEYUP)
				if(keyboardInput())
					return true;
			// Always check mouse
			checkMouse(e);			
		}

		return false;
	}
	// Drawing functions
	void drawLine(int x1, int y1, int x2, int y2)
	{
		SDL_RenderDrawLine(rend, x1, y1, x2, y2);
	}
	void drawPoint(int x, int y)
	{
		SDL_RenderDrawPoint(rend, x, y);
	}
	void drawRect(int x, int y, int w, int h)
	{
		SDL_Rect rect;
		rect.x = x;
		rect.y = y;
		rect.w = w;
		rect.h = h;
		SDL_RenderDrawRect(rend, &rect);
	}
	void fillRect(int x, int y, int w, int h)
	{
		SDL_Rect rect;
		rect.x = x;
		rect.y = y;
		rect.w = w;
		rect.h = h;
		SDL_RenderFillRect(rend, &rect);
	}
	void drawText(int x, int y, int w, int h, const char* mes, int size, int i)
	{
		int extra = strlen(mes);
		if(message[i] == NULL) {
			TTF_Font* Sans = TTF_OpenFont("./Roboto-Black.ttf", size+extra);
			if(Sans == NULL) {
				printf("ERROR FONT: %s\n", SDL_GetError());
				return;
			}
			SDL_Color BLACK = {0,0,0};
			SDL_Surface* surface = TTF_RenderText_Solid(Sans, mes, BLACK);
			message[i] = SDL_CreateTextureFromSurface(rend, surface);
			SDL_FreeSurface(surface);
		}
		SDL_Rect Message_rect;
		Message_rect.x = x+extra;
		Message_rect.y = y+extra;
		Message_rect.w = w+extra;
		Message_rect.h = h;
		SDL_RenderCopy(rend, message[i], NULL, &Message_rect);
	}
} s;

void deleteParticles()
{
	global.lbutton_down = false;
	global.rbutton_down = true;
	float mouse[2] = {(float)global.mousex, (float)global.mousey};
	
	for(int i=0; i<global.pcount; i++) {
		float pos[2] = {global.p[i].x, global.p[i].y};
		float dist = getLength(pos, mouse);
		if(dist < global.radius)
			global.p[i].doKill = true;
	}

	deallocateParticles();
}

void generateParticles()
{
	global.lbutton_down = true;
	global.rbutton_down = false;
	if(global.show_menu && global.mousey < 100) return;
	if(!global.inbounds) return;
	float mouse[2] = {(float)global.mousex, (float)global.mousey};
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
	if(closest_dist < global.radius) {
		allocateParticle(global.select,
			       g.cells[celly_idx][cellx_idx].x,
		       	   g.cells[celly_idx][cellx_idx].y,
				   cellx_idx, celly_idx);
	}

}

void updateCellInfo(int idx)
{
	int cellx = global.p[idx].cellx;
	int celly = global.p[idx].celly;

	g.cells[celly][cellx].taken = true;
	g.cells[celly][cellx].id = global.p[idx].id;
	g.cells[celly][cellx].idx = idx;
}

void changeParticle(Particle req, int idx)
{
	// Save coords
	float x = global.p[idx].x;
	float y = global.p[idx].y;
	// Save old cell
	int cellx = global.p[idx].cellx;
	int celly = global.p[idx].celly;
	// Assign
	global.p[idx] = req;
	global.p[idx].cellx = cellx;
	global.p[idx].celly = celly;
	// Update cell info
	updateCellInfo(idx);
	// Restore old necessary info
	global.p[idx].x = x;
	global.p[idx].y = y;
	global.p[idx].idx = idx;
}

// Begin - How each particle type reacts with its environment

void updateWater(int idx)
{
	if(global.p[idx].color[2] < 200) {
		// Too much salt dissolves water
		global.p[idx].doKill = true;
		return;
	}
	const short int ACC = global.p[idx].Tacc;
	// Position of current cell
	int posx = global.p[idx].cellx;
	int posy = global.p[idx].celly;
	updateCellInfo(idx);
	// Calculate possible locations
	int next_row = posy + global.p[idx].vel[1];
	if(next_row >= g.max_rows)
		next_row = posy;
	int prev_row = posy - global.p[idx].vel[1];
	if(prev_row < 0)
		prev_row = posy;
	int next_col = posx + global.p[idx].vel[0];
	if(next_col >= g.max_cols)
		next_col = posx;
	int prev_col = posx - global.p[idx].vel[0];
	if(prev_col < 0)
		prev_col = posx;

	// Water touches fire -> Steam
	// Destroy water, fire turns to steam
	if(g.cells[posy][prev_col].id == FIRE && g.cells[posy][prev_col].taken) {
		// Kill water
		global.p[idx].doKill = true;
		// Change fire to steam
		int idx1 = g.cells[posy][prev_col].idx;
		if(idx1 >= 0 && idx1 < global.pcount) {
			global.p[idx1].id = STEAM;
			global.p[idx1].reinit();
			updateCellInfo(idx1);
		}
		return;
	} else if(g.cells[posy][next_col].id == FIRE && g.cells[posy][next_col].taken) {
		// Kill water
		global.p[idx].doKill = true;
		// Change fire to steam
		int idx1 = g.cells[posy][next_col].idx;
		if(idx1 >= 0 && idx1 < global.pcount) {
			global.p[idx1].id = STEAM;
			global.p[idx1].reinit();
			updateCellInfo(idx1);
		}
		return;
	} else if(g.cells[prev_row][posx].id == FIRE && g.cells[prev_row][posx].taken) {
		// Kill water
		global.p[idx].doKill = true;
		// Change fire to steam
		int idx1 = g.cells[prev_row][posx].idx;
		if(idx1 >= 0 && idx1 < global.pcount) {
			global.p[idx1].id = STEAM;
			global.p[idx1].reinit();
			updateCellInfo(idx1);
		}
		return;
	} else if(g.cells[next_row][posx].id == FIRE && g.cells[next_row][posx].taken) {
		// Kill water
		global.p[idx].doKill = true;
		// Change fire to steam
		int idx1 = g.cells[next_row][posx].idx;
		if(idx1 >= 0 && idx1 < global.pcount) {
			global.p[idx1].id = STEAM;
			global.p[idx1].reinit();
			updateCellInfo(idx1);
		}
		return;
	}
	
	// Salt evaporates with water
	if(g.cells[prev_row][posx].id == SALT && g.cells[next_row][posx].taken) {
		// Above
		global.p[idx].color[2] -= 5;
		global.p[idx].color[1] += 2;

		int idx1 = g.cells[prev_row][posx].idx;
		global.p[idx1].color[0]--;
	} else if(g.cells[next_row][posx].id == SALT && g.cells[next_row][posx].taken) {
		// Below
		global.p[idx].color[2] -= 5;
		global.p[idx].color[1] += 2;
		// Salt not updating when water fell above?
		int idx1 = g.cells[next_row][posx].idx;
		global.p[idx1].color[0]--;
	} else if(g.cells[posy][next_col].id == SALT && g.cells[posy][next_col].taken) {
		// Right
		global.p[idx].color[2] -= 5;
		global.p[idx].color[1] += 2;
		
		int idx1 = g.cells[posy][next_col].idx;
		global.p[idx1].color[0]--;
	} else if(g.cells[posy][prev_col].id == SALT && g.cells[posy][prev_col].taken) {
		// Left
		global.p[idx].color[2] -= 5;
		global.p[idx].color[1] += 2;
		
		int idx1 = g.cells[posy][prev_col].idx;
		global.p[idx1].color[0]--;
	}

	// Update logic for water
	if(!g.cells[next_row][posx].taken) {
		// Water falls directly down
		if(global.p[idx].acc < ACC)
			global.p[idx].acc++;
		int dist = 0;
		while(!g.cells[next_row][posx].taken && dist < global.p[idx].acc) {
			// Set current cell to not taken & take next spot
			g.cells[posy][posx].taken = false;
			g.cells[posy][posx].idx = -1;
			g.cells[posy][posx].id = -1;
			g.cells[next_row][posx].taken = true;
			// Update current spot
			global.p[idx].celly = next_row;
			posy = global.p[idx].celly;
			// Update coords
			global.p[idx].x = g.cells[posy][posx].x;
			global.p[idx].y = g.cells[posy][posx].y;
			// Calculate possible locations
			next_row = posy + global.p[idx].vel[1];
			if(next_row >= g.max_rows)
				next_row = posy;
			prev_row = posy - global.p[idx].vel[1];
			if(prev_row < 0)
				prev_row = posy;
			next_col = posx + global.p[idx].vel[0];
			if(next_col >= g.max_cols)
				next_col = posx;
			prev_col = posx - global.p[idx].vel[0];
			if(prev_col < 0)
				prev_col = posx;
			dist++;
		}
		return;
	} else if(!g.cells[next_row][next_col].taken) {
		// Water falls down->right
		if(global.p[idx].acc < ACC)
			global.p[idx].acc++;
		int dist = 0;
		while(!g.cells[next_row][next_col].taken && dist < global.p[idx].acc) {
			// Set current cell to not taken & take next spot
			g.cells[posy][posx].taken = false;
			g.cells[posy][posx].idx = -1;
			g.cells[posy][posx].id = -1;
			g.cells[next_row][next_col].taken = true;
			// Update current spot
			global.p[idx].celly = next_row;
			global.p[idx].cellx = next_col;
			posy = global.p[idx].celly;
			posx = global.p[idx].cellx;
			// Update coords
			global.p[idx].x = g.cells[posy][posx].x;
			global.p[idx].y = g.cells[posy][posx].y;
			// Calculate possible locations
			next_row = posy + global.p[idx].vel[1];
			if(next_row >= g.max_rows)
				next_row = posy;
			prev_row = posy - global.p[idx].vel[1];
			if(prev_row < 0)
				prev_row = posy;
			next_col = posx + global.p[idx].vel[0];
			if(next_col >= g.max_cols)
				next_col = posx;
			prev_col = posx - global.p[idx].vel[0];
			if(prev_col < 0)
				prev_col = posx;
			dist++;
		}
		return;
	} else if(!g.cells[next_row][prev_col].taken) {
		// Water falls down->left
		if(global.p[idx].acc < ACC)
			global.p[idx].acc++;
		int dist = 0;
		while(!g.cells[next_row][prev_col].taken && dist < global.p[idx].acc) {
			// Set current cell to not taken & take next spot
			g.cells[posy][posx].taken = false;
			g.cells[posy][posx].idx = -1;
			g.cells[posy][posx].id = -1;
			g.cells[next_row][prev_col].taken = true;
			// Update current spot
			global.p[idx].celly = next_row;
			global.p[idx].cellx = prev_col;
			posy = global.p[idx].celly;
			posx = global.p[idx].cellx;
			// Update coords
			global.p[idx].x = g.cells[posy][posx].x;
			global.p[idx].y = g.cells[posy][posx].y;
			// Calculate possible locations
			next_row = posy + global.p[idx].vel[1];
			if(next_row >= g.max_rows)
				next_row = posy;
			prev_row = posy - global.p[idx].vel[1];
			if(prev_row < 0)
				prev_row = posy;
			next_col = posx + global.p[idx].vel[0];
			if(next_col >= g.max_cols)
				next_col = posx;
			prev_col = posx - global.p[idx].vel[0];
			if(prev_col < 0)
				prev_col = posx;
			dist++;
		}
		return;
	} else if(!g.cells[posy][next_col].taken) {
		// Water flows to the right
		global.p[idx].acc = 1;
		int dist = 0;
		// Set current cell to not taken & take next spot
		g.cells[posy][posx].taken = false;
		g.cells[posy][posx].idx = -1;
		g.cells[posy][posx].id = -1;
		g.cells[posy][next_col].taken = true;
		// Update current spot
		global.p[idx].cellx = next_col;
		updateCellInfo(idx);
		posx = global.p[idx].cellx;
		// Update coords
		global.p[idx].x = g.cells[posy][posx].x;
		global.p[idx].y = g.cells[posy][posx].y;
	} else if(!g.cells[posy][prev_col].taken) {
		// Water flows to the left
		global.p[idx].acc = 1;
		int dist = 0;
		// Set current cell to not taken & take next spot
		g.cells[posy][posx].taken = false;
		g.cells[posy][posx].idx = -1;
		g.cells[posy][posx].id = -1;
		g.cells[posy][prev_col].taken = true;
		// Update current spot
		global.p[idx].cellx = prev_col;
		updateCellInfo(idx);
		posx = global.p[idx].cellx;
		// Update coords
		global.p[idx].x = g.cells[posy][posx].x;
		global.p[idx].y = g.cells[posy][posx].y;
	} else {
		// This object has stopped
		// Restart acc
		global.p[idx].acc = 0;
	}
}

void updateSand(int idx)
{
	const short int ACC = global.p[idx].Tacc;
	// Position of current cell
	int posx = global.p[idx].cellx;
	int posy = global.p[idx].celly;
	updateCellInfo(idx);
	// Calculate possible locations
	int next_row = posy + global.p[idx].vel[1];
	if(next_row >= g.max_rows)
		return;
	int next_col = posx + global.p[idx].vel[0];
	if(next_col >= g.max_cols)
		next_col = posx;
	int prev_col = posx - global.p[idx].vel[0];
	if(prev_col < 0)
		prev_col = posx;

	// Sand sinks to the bottom of water
	if(g.cells[next_row][posx].id == WATER && g.cells[next_row][posx].taken) {
		if(!g.cells[posy][next_col].taken) {
			// Water pos
			int idx1 = g.cells[next_row][posx].idx;
			global.p[idx1].x = g.cells[posy][next_col].x;
			global.p[idx1].y = g.cells[posy][next_col].y;
			int cellx = global.p[idx1].cellx;
			int celly = global.p[idx1].celly;
			g.cells[celly][cellx].taken = false;
			g.cells[celly][cellx].id = -1;
			g.cells[celly][cellx].idx = -1;
			global.p[idx1].cellx = next_col;
			global.p[idx1].celly = posy;
			updateCellInfo(idx1);
		} else if(!g.cells[posy][prev_col].taken) {
			// Water pos
			int idx1 = g.cells[next_row][posx].idx;
			global.p[idx1].x = g.cells[posy][prev_col].x;
			global.p[idx1].y = g.cells[posy][prev_col].y;
			int cellx = global.p[idx1].cellx;
			int celly = global.p[idx1].celly;
			g.cells[celly][cellx].taken = false;
			g.cells[celly][cellx].id = -1;
			g.cells[celly][cellx].idx = -1;
			global.p[idx1].cellx = prev_col;
			global.p[idx1].celly = posy;
			updateCellInfo(idx1);
		} else {
			int idx1 = g.cells[next_row][posx].idx;
			Particle tmp = global.p[idx];
			changeParticle(global.p[idx1], idx);
			changeParticle(tmp, idx1);
		}
		return;
	} else if(g.cells[next_row][prev_col].id == WATER && g.cells[next_row][prev_col].taken) {
		int idx1 = g.cells[next_row][prev_col].idx;
		Particle tmp = global.p[idx];
		changeParticle(global.p[idx1], idx);
		changeParticle(tmp, idx1);
		return;
	} else if(g.cells[next_row][next_col].id == WATER && g.cells[next_row][next_col].taken) {
		int idx1 = g.cells[next_row][next_col].idx;
		Particle tmp = global.p[idx];
		changeParticle(global.p[idx1], idx);
		changeParticle(tmp, idx1);
		return;
	}

	// Update logic for sand
	if(!g.cells[next_row][posx].taken) {
		// Sand falls directly down
		if(global.p[idx].acc < ACC)
			global.p[idx].acc++;
		int dist = 0;
		while(!g.cells[next_row][posx].taken && dist < global.p[idx].acc) {
			// Set current cell to not taken & take next spot
			g.cells[posy][posx].taken = false;
			g.cells[posy][posx].idx = -1;
			g.cells[posy][posx].id = -1;
			g.cells[next_row][posx].taken = true;
			// Update current spot
			global.p[idx].celly = next_row;
			posy = global.p[idx].celly;
			// Update coords
			global.p[idx].x = g.cells[posy][posx].x;
			global.p[idx].y = g.cells[posy][posx].y;
			// Calculate possible locations
			next_row = posy + global.p[idx].vel[1];
			if(next_row >= g.max_rows)
				return;
			next_col = posx + global.p[idx].vel[0];
			if(next_col >= g.max_cols)
				next_col = posx;
			prev_col = posx - global.p[idx].vel[0];
			if(prev_col < 0)
				prev_col = posx;
			dist++;
		}
		return;
	} else if(!g.cells[next_row][next_col].taken) {
		// Sand falls down->right
		if(global.p[idx].acc < ACC)
			global.p[idx].acc++;
		int dist = 0;
		while(!g.cells[next_row][next_col].taken && dist < global.p[idx].acc) {
			// Set current cell to not taken & take next spot
			g.cells[posy][posx].taken = false;
			g.cells[posy][posx].idx = -1;
			g.cells[posy][posx].id = -1;
			g.cells[next_row][next_col].taken = true;
			// Update current spot
			global.p[idx].celly = next_row;
			global.p[idx].cellx = next_col;
			posy = global.p[idx].celly;
			posx = global.p[idx].cellx;
			// Update coords
			global.p[idx].x = g.cells[posy][posx].x;
			global.p[idx].y = g.cells[posy][posx].y;
			// Calculate possible locations
			next_row = posy + global.p[idx].vel[1];
			if(next_row >= g.max_rows)
				return;
			next_col = posx + global.p[idx].vel[0];
			if(next_col >= g.max_cols)
				next_col = posx;
			prev_col = posx - global.p[idx].vel[0];
			if(prev_col < 0)
				prev_col = posx;
			dist++;
		}
		return;
	} else if(!g.cells[next_row][prev_col].taken) {
		// Sand falls down->left
		if(global.p[idx].acc < ACC)
			global.p[idx].acc++;
		int dist = 0;
		while(!g.cells[next_row][prev_col].taken && dist < global.p[idx].acc) {
			// Set current cell to not taken & take next spot
			g.cells[posy][posx].taken = false;
			g.cells[posy][posx].idx = -1;
			g.cells[posy][posx].id = -1;
			g.cells[next_row][prev_col].taken = true;
			// Update current spot
			global.p[idx].celly = next_row;
			global.p[idx].cellx = prev_col;
			posy = global.p[idx].celly;
			posx = global.p[idx].cellx;
			// Update coords
			global.p[idx].x = g.cells[posy][posx].x;
			global.p[idx].y = g.cells[posy][posx].y;
			// Calculate possible locations
			next_row = posy + global.p[idx].vel[1];
			if(next_row >= g.max_rows)
				return;
			next_col = posx + global.p[idx].vel[0];
			if(next_col >= g.max_cols)
				next_col = posx;
			prev_col = posx - global.p[idx].vel[0];
			if(prev_col < 0)
				prev_col = posx;
			dist++;
		}
		return;
	} else {
		// This object has stopped
		// Restart acc
		global.p[idx].acc = 0;
	}
}

void updateWood(int idx)
{
	// Wood stays in place
	// Position of current cell
	updateCellInfo(idx);
}

void updateFire(int idx)
{
	// Update color
	global.p[idx].color[0] -= 5;
	global.p[idx].color[1] -= 5;
	if(global.p[idx].color[1] < 50) {
	//	printf("Set doKill\n");
		global.p[idx].doKill = true;
		return;
	}
	// Position of current cell
	int posx = global.p[idx].cellx;
	int posy = global.p[idx].celly;
	updateCellInfo(idx);
	// Calculate possible locations
	int next_row = posy + global.p[idx].vel[1];
	if(next_row >= g.max_rows)
		next_row = 0;
	int prev_row = posy - global.p[idx].vel[1];
	if(prev_row < 0)
		prev_row = 0;
	int next_col = posx + global.p[idx].vel[0];
	if(next_col >= g.max_cols)
		next_col = posx;
	int prev_col = posx - global.p[idx].vel[0];
	if(prev_col < 0)
		prev_col = posx;

	// Falling sand puts fire out
	if(g.cells[prev_row][posx].id == SAND && g.cells[prev_row][posx].taken) {
		global.p[idx].doKill = true;
		return;	
	}

	// Only spread to wood
	// First, left or right
	if(g.cells[posy][prev_col].id == WOOD && g.cells[posy][prev_col].taken) {
		changeParticle(global.p[idx], g.cells[posy][prev_col].idx);
		int leftside = prev_col-1 >= 0 ? prev_col-1 : prev_col;
		int rightside = next_col+1 < global.xres ? next_col+1 : next_col;
		// Generate smoke when wood burns
		if(!g.cells[prev_row][prev_col].taken) {
			int xpos = g.cells[prev_row][prev_col].x;	
			int ypos = g.cells[prev_row][prev_col].y;
			allocateParticle(SMOKE, xpos, ypos, prev_col, prev_row);
		}
		if(!g.cells[posy][leftside].taken) {
			int xpos = g.cells[posy][leftside].x;
			int ypos = g.cells[posy][leftside].y;
			allocateParticle(SMOKE, xpos, ypos, leftside, posy);
		}
		if(!g.cells[posy][rightside].taken) {
			int xpos = g.cells[posy][rightside].x;
			int ypos = g.cells[posy][rightside].y;
			allocateParticle(SMOKE, xpos, ypos, rightside, posy);
		}
	}
	if(g.cells[posy][next_col].id == WOOD && g.cells[posy][next_col].taken) {
		changeParticle(global.p[idx], g.cells[posy][next_col].idx);
		int leftside = prev_col-1 >= 0 ? prev_col-1 : prev_col;
		int rightside = next_col+1 < global.xres ? next_col+1 : next_col;
		// Generate smoke when wood burns
		if(!g.cells[prev_row][prev_col].taken) {
			int xpos = g.cells[prev_row][next_col].x;	
			int ypos = g.cells[prev_row][next_col].y;
			allocateParticle(SMOKE, xpos, ypos, prev_col, prev_row);
		}
		if(!g.cells[posy][leftside].taken) {
			int xpos = g.cells[posy][leftside].x;
			int ypos = g.cells[posy][leftside].y;
			allocateParticle(SMOKE, xpos, ypos, leftside, posy);
		}
		if(!g.cells[posy][rightside].taken) {
			int xpos = g.cells[posy][rightside].x;
			int ypos = g.cells[posy][rightside].y;
			allocateParticle(SMOKE, xpos, ypos, rightside, posy);
		}
	}
	// Then up or down
	if(g.cells[prev_row][posx].id == WOOD && g.cells[prev_row][posx].taken) {
		changeParticle(global.p[idx], g.cells[prev_row][posx].idx);
		int leftside = posx-1 >= 0 ? posx-1 : posx;
		int rightside = posx+1 < global.xres ? posx+1 : posx;
		int above = prev_row-1 >= 0 ? prev_row-1 : prev_row;
		// Generate smoke when wood burns
		if(!g.cells[above][posx].taken) {
			int xpos = g.cells[above][posx].x;	
			int ypos = g.cells[above][posx].y;
			allocateParticle(SMOKE, xpos, ypos, posx, above);
		}
		if(!g.cells[prev_row][leftside].taken) {
			int xpos = g.cells[prev_row][leftside].x;
			int ypos = g.cells[prev_row][leftside].y;
			allocateParticle(SMOKE, xpos, ypos, leftside, prev_row);
		}
		if(!g.cells[prev_row][rightside].taken) {
			int xpos = g.cells[prev_row][rightside].x;
			int ypos = g.cells[prev_row][rightside].y;
			allocateParticle(SMOKE, xpos, ypos, rightside, prev_row);
		}
	}
	if(g.cells[next_row][posx].id == WOOD && g.cells[next_row][posx].taken) {
		changeParticle(global.p[idx], g.cells[next_row][posx].idx);
		int leftside = posx-1 >= 0 ? posx-1 : posx;
		int rightside = posx+1 < global.xres ? posx+1 : posx;
		int below = next_row+1 >= 0 ? next_row+1 : next_row;
		// Generate smoke when wood burns
		if(!g.cells[below][posx].taken) {
			int xpos = g.cells[below][posx].x;	
			int ypos = g.cells[below][posx].y;
			allocateParticle(SMOKE, xpos, ypos, posx, below);
		}
		if(!g.cells[prev_row][leftside].taken) {
			int xpos = g.cells[prev_row][leftside].x;
			int ypos = g.cells[prev_row][leftside].y;
			allocateParticle(SMOKE, xpos, ypos, leftside, prev_row);
		}
		if(!g.cells[prev_row][rightside].taken) {
			int xpos = g.cells[prev_row][rightside].x;
			int ypos = g.cells[prev_row][rightside].y;
			allocateParticle(SMOKE, xpos, ypos, rightside, prev_row);
		}
	}
}

void updateSmoke(int idx)
{
	// Update color
	global.p[idx].color[0] -= 3;
	global.p[idx].color[1] -= 3;
	global.p[idx].color[2] -= 3;
	if(global.p[idx].color[0] < 25) {
		global.p[idx].doKill = true;
		return;
	}
	int posx = global.p[idx].cellx;
	int posy = global.p[idx].celly;
	updateCellInfo(idx);
	// Calculate possible locations
	int next_row = posy + global.p[idx].vel[1];
	if(next_row >= g.max_rows)
		next_row = posy;
	int prev_row = posy - global.p[idx].vel[1];
	if(prev_row < 0)
		prev_row = posy;
	int next_col = posx + global.p[idx].vel[0];
	if(next_col >= g.max_cols)
		next_col = posx;
	int prev_col = posx - global.p[idx].vel[0];
	if(prev_col < 0)
		prev_col = posx;

	// Always move smoke up
	if(!g.cells[prev_row][posx].taken) {
		// Set current cell to not taken & take next spot
		g.cells[posy][posx].taken = false;
		g.cells[posy][posx].idx = -1;
		g.cells[posy][posx].id = -1;
		g.cells[prev_row][posx].taken = true;
		// Update current spot
		global.p[idx].celly = prev_row;
		posy = global.p[idx].celly;
		// Update coords
		global.p[idx].x = g.cells[posy][posx].x;
		global.p[idx].y = g.cells[posy][posx].y;
		updateCellInfo(idx);
	} else if(!g.cells[prev_row][prev_col].taken) {
		// Set current cell to not taken & take next spot
		g.cells[posy][posx].taken = false;
		g.cells[posy][posx].idx = -1;
		g.cells[posy][posx].id = -1;
		g.cells[prev_row][prev_col].taken = true;
		// Update current spot
		global.p[idx].celly = prev_row;
		global.p[idx].cellx = prev_col;
		posy = global.p[idx].celly;
		posx = global.p[idx].cellx;
		// Update coords
		global.p[idx].x = g.cells[posy][posx].x;
		global.p[idx].y = g.cells[posy][posx].y;
		updateCellInfo(idx);
		return;
	} else if(!g.cells[prev_row][next_col].taken) {
		// Set current cell to not taken & take next spot
		g.cells[posy][posx].taken = false;
		g.cells[posy][posx].idx = -1;
		g.cells[posy][posx].id = -1;
		g.cells[prev_row][next_col].taken = true;
		// Update current spot
		global.p[idx].celly = prev_row;
		global.p[idx].cellx = next_col;
		posy = global.p[idx].celly;
		posx = global.p[idx].cellx;
		// Update coords
		global.p[idx].x = g.cells[posy][posx].x;
		global.p[idx].y = g.cells[posy][posx].y;
		updateCellInfo(idx);
		return;
	} else if(!g.cells[posy][prev_col].taken) {
		// Smoke flows to the left
		// Set current cell to not taken & take next spot
		g.cells[posy][posx].taken = false;
		g.cells[posy][posx].idx = -1;
		g.cells[posy][posx].id = -1;
		g.cells[posy][prev_col].taken = true;
		// Update current spot
		global.p[idx].cellx = prev_col;
		posy = global.p[idx].celly;
		posx = global.p[idx].cellx;
		// Update coords
		global.p[idx].x = g.cells[posy][posx].x;
		global.p[idx].y = g.cells[posy][posx].y;
	} else if(!g.cells[posy][next_col].taken) {
		// Smoke flows to the right
		// Set current cell to not taken & take next spot
		g.cells[posy][posx].taken = false;
		g.cells[posy][posx].idx = -1;
		g.cells[posy][posx].id = -1;
		g.cells[posy][next_col].taken = true;
		// Update current spot
		global.p[idx].cellx = next_col;
		posy = global.p[idx].celly;
		posx = global.p[idx].cellx;
		// Update coords
		global.p[idx].x = g.cells[posy][posx].x;
		global.p[idx].y = g.cells[posy][posx].y;
	}
	
	// Create smoke
	static int doMake = 0;
	doMake++;
	if(doMake < 75) return;
	else doMake = 0;
	// Calculate possible locations
	next_row = posy + global.p[idx].vel[1];
	if(next_row >= g.max_rows)
		next_row = posy;
	prev_row = posy - global.p[idx].vel[1];
	if(prev_row < 0)
		prev_row = posy;
	next_col = posx + global.p[idx].vel[0];
	if(next_col >= g.max_cols)
		next_col = posx;
	prev_col = posx - global.p[idx].vel[0];
	if(prev_col < 0)
		prev_col = posx;
	
	if(!g.cells[prev_row][prev_col].taken) {
		float xpos = g.cells[prev_row][prev_col].x;
		float ypos = g.cells[prev_row][prev_col].y;
		allocateParticle(SMOKE, xpos, ypos, prev_col, prev_row);
	} else if(!g.cells[prev_row][next_col].taken) {
		float xpos = g.cells[prev_row][next_col].x;
		float ypos = g.cells[prev_row][next_col].y;
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
	global.p[idx].color[0]--;
	global.p[idx].color[1]--;
	global.p[idx].color[2]--;
	if(global.p[idx].color[0] < 140) {
		global.p[idx].doKill = true;
		return;
	}
	int posx = global.p[idx].cellx;
	int posy = global.p[idx].celly;
	updateCellInfo(idx);
	// Calculate possible locations
	int next_row = posy + global.p[idx].vel[1];
	if(next_row >= g.max_rows)
		next_row = posy;
	int prev_row = posy - global.p[idx].vel[1];
	if(prev_row < 0)
		prev_row = posy;
	int next_col = posx + global.p[idx].vel[0];
	if(next_col >= g.max_cols)
		next_col = posx;
	int prev_col = posx - global.p[idx].vel[0];
	if(prev_col < 0)
		prev_col = posx;

	// Always move steam up
	if(!g.cells[prev_row][posx].taken) {
		// Set current cell to not taken & take next spot
		g.cells[posy][posx].taken = false;
		g.cells[posy][posx].idx = -1;
		g.cells[posy][posx].id = -1;
		g.cells[prev_row][posx].taken = true;
		// Update current spot
		global.p[idx].celly = prev_row;
		posy = global.p[idx].celly;
		// Update coords
		global.p[idx].x = g.cells[posy][posx].x;
		global.p[idx].y = g.cells[posy][posx].y;
		updateCellInfo(idx);
		return;
	} else if(!g.cells[prev_row][prev_col].taken) {
		// Set current cell to not taken & take next spot
		g.cells[posy][posx].taken = false;
		g.cells[posy][posx].idx = -1;
		g.cells[posy][posx].id = -1;
		g.cells[prev_row][prev_col].taken = true;
		// Update current spot
		global.p[idx].celly = prev_row;
		global.p[idx].cellx = prev_col;
		posy = global.p[idx].celly;
		posx = global.p[idx].cellx;
		// Update coords
		global.p[idx].x = g.cells[posy][posx].x;
		global.p[idx].y = g.cells[posy][posx].y;
		updateCellInfo(idx);
		return;
	} else if(!g.cells[prev_row][next_col].taken) {
		// Set current cell to not taken & take next spot
		g.cells[posy][posx].taken = false;
		g.cells[posy][posx].idx = -1;
		g.cells[posy][posx].id = -1;
		g.cells[prev_row][next_col].taken = true;
		// Update current spot
		global.p[idx].celly = prev_row;
		global.p[idx].cellx = next_col;
		posy = global.p[idx].celly;
		posx = global.p[idx].cellx;
		// Update coords
		global.p[idx].x = g.cells[posy][posx].x;
		global.p[idx].y = g.cells[posy][posx].y;
		updateCellInfo(idx);
		return;
	} else if(!g.cells[posy][prev_col].taken) {
		// Smoke flows to the left
		// Set current cell to not taken & take next spot
		g.cells[posy][posx].taken = false;
		g.cells[posy][posx].idx = -1;
		g.cells[posy][posx].id = -1;
		g.cells[posy][prev_col].taken = true;
		// Update current spot
		global.p[idx].cellx = prev_col;
		posy = global.p[idx].celly;
		posx = global.p[idx].cellx;
		// Update coords
		global.p[idx].x = g.cells[posy][posx].x;
		global.p[idx].y = g.cells[posy][posx].y;
	} else if(!g.cells[posy][next_col].taken) {
		// Smoke flows to the right
		// Set current cell to not taken & take next spot
		g.cells[posy][posx].taken = false;
		g.cells[posy][posx].idx = -1;
		g.cells[posy][posx].id = -1;
		g.cells[posy][next_col].taken = true;
		// Update current spot
		global.p[idx].cellx = next_col;
		posy = global.p[idx].celly;
		posx = global.p[idx].cellx;
		// Update coords
		global.p[idx].x = g.cells[posy][posx].x;
		global.p[idx].y = g.cells[posy][posx].y;
	}

	// If nothing around, create more steam
	posx = global.p[idx].cellx;
	posy = global.p[idx].celly;
	updateCellInfo(idx);
	// Calculate possible locations
	next_row = posy + global.p[idx].vel[1];
	if(next_row >= g.max_rows)
		next_row = posy;
	prev_row = posy - global.p[idx].vel[1];
	if(prev_row < 0)
		prev_row = posy;
	next_col = posx + global.p[idx].vel[0];
	if(next_col >= g.max_cols)
		next_col = posx;
	prev_col = posx - global.p[idx].vel[0];
	if(prev_col < 0)
		prev_col = posx;
	
	// Create steam
	if(!g.cells[prev_row][posx].taken) {
		float xpos = g.cells[prev_row][posx].x;
		float ypos = g.cells[prev_row][posx].y;
		allocateParticle(STEAM, xpos, ypos, posx, prev_row);
	} if(!g.cells[prev_row][prev_col].taken) {
		float xpos = g.cells[prev_row][prev_col].x;
		float ypos = g.cells[prev_row][prev_col].y;
		allocateParticle(STEAM, xpos, ypos, prev_col, prev_row);
	} if(!g.cells[prev_row][next_col].taken) {
		float xpos = g.cells[prev_row][next_col].x;
		float ypos = g.cells[prev_row][next_col].y;
		allocateParticle(STEAM, xpos, ypos, next_col, prev_row);
	}
}

void updateSalt(int idx)
{
	const short int ACC = global.p[idx].Tacc;
	if(global.p[idx].color[0] < 255) {
		global.p[idx].doKill = true;
		return;
	}
	bool do_return = false;
	// Position of current cell
	int posx = global.p[idx].cellx;
	int posy = global.p[idx].celly;
	updateCellInfo(idx);
	// Calculate possible locations
	int next_row = posy + global.p[idx].vel[1];
	if(next_row >= g.max_rows)
		return;
	int prev_row = posy - global.p[idx].vel[1];
	if(prev_row < 0)
		prev_row = posy;
	int next_col = posx + global.p[idx].vel[0];
	if(next_col >= g.max_cols)
		next_col = posx;
	int prev_col = posx - global.p[idx].vel[0];
	if(prev_col < 0)
		prev_col = posx;

	// Salt evaporates with water
	if(g.cells[prev_row][posx].id == WATER) {
		// Above
		global.p[idx].color[0] -= 5;
	} else if(g.cells[next_row][posx].id == WATER && g.cells[next_row][posx].taken) {
		// Below
		global.p[idx].color[0] -= 5;
	} else if(g.cells[posy][next_col].id == WATER && g.cells[posy][next_col].taken) {
		// Right
		global.p[idx].color[0] -= 5;
	} else if(g.cells[posy][prev_col].id == WATER && g.cells[posy][prev_col].taken) {
		// Left
		global.p[idx].color[0] -= 5;
	}
	// Update logic for salt
	if(!g.cells[next_row][posx].taken) {
		// Salt falls directly down
		if(global.p[idx].acc < ACC)
			global.p[idx].acc++;
		int dist = 0;
		while(!g.cells[next_row][posx].taken && dist < global.p[idx].acc) {
			// Set current cell to not taken & take next spot
			g.cells[posy][posx].taken = false;
			g.cells[posy][posx].idx = -1;
			g.cells[posy][posx].id = -1;
			g.cells[next_row][posx].taken = true;
			// Update current spot
			global.p[idx].celly = next_row;
			posy = global.p[idx].celly;
			// Update coords
			global.p[idx].x = g.cells[posy][posx].x;
			global.p[idx].y = g.cells[posy][posx].y;
			// Calculate possible locations
			next_row = posy + global.p[idx].vel[1];
			if(next_row >= g.max_rows)
				return;
			next_col = posx + global.p[idx].vel[0];
			if(next_col >= g.max_cols)
				next_col = posx;
			prev_col = posx - global.p[idx].vel[0];
			if(prev_col < 0)
				prev_col = posx;
			dist++;
		}
		return;
	} else if(!g.cells[next_row][next_col].taken) {
		// Salt falls down->right
		if(global.p[idx].acc < ACC)
			global.p[idx].acc++;
		int dist = 0;
		while(!g.cells[next_row][next_col].taken && dist < global.p[idx].acc) {
			// Set current cell to not taken & take next spot
			g.cells[posy][posx].taken = false;
			g.cells[posy][posx].idx = -1;
			g.cells[posy][posx].id = -1;
			g.cells[next_row][next_col].taken = true;
			// Update current spot
			global.p[idx].celly = next_row;
			global.p[idx].cellx = next_col;
			posy = global.p[idx].celly;
			posx = global.p[idx].cellx;
			// Update coords
			global.p[idx].x = g.cells[posy][posx].x;
			global.p[idx].y = g.cells[posy][posx].y;
			// Calculate possible locations
			next_row = posy + global.p[idx].vel[1];
			if(next_row >= g.max_rows)
				return;
			next_col = posx + global.p[idx].vel[0];
			if(next_col >= g.max_cols)
				next_col = posx;
			prev_col = posx - global.p[idx].vel[0];
			if(prev_col < 0)
				prev_col = posx;
			dist++;
		}
		return;
	} else if(!g.cells[next_row][prev_col].taken) {
		// Salt falls down->left
		if(global.p[idx].acc < ACC)
			global.p[idx].acc++;
		int dist = 0;
		while(!g.cells[next_row][prev_col].taken && dist < global.p[idx].acc) {
			// Set current cell to not taken & take next spot
			g.cells[posy][posx].taken = false;
			g.cells[posy][posx].idx = -1;
			g.cells[posy][posx].id = -1;
			g.cells[next_row][prev_col].taken = true;
			// Update current spot
			global.p[idx].celly = next_row;
			global.p[idx].cellx = prev_col;
			posy = global.p[idx].celly;
			posx = global.p[idx].cellx;
			// Update coords
			global.p[idx].x = g.cells[posy][posx].x;
			global.p[idx].y = g.cells[posy][posx].y;
			// Calculate possible locations
			next_row = posy + global.p[idx].vel[1];
			if(next_row >= g.max_rows)
				return;
			next_col = posx + global.p[idx].vel[0];
			if(next_col >= g.max_cols)
				next_col = posx;
			prev_col = posx - global.p[idx].vel[0];
			if(prev_col < 0)
				prev_col = posx;
			dist++;
		}
		return;
	} else {
		// This object has stopped
		// Restart acc
		global.p[idx].acc = 0;
	}
}

void updateMetal(int idx)
{
	// Stays in place, but interacts with fire & water
	const short int MAX_HEAT = 240;
	const short int MIN_HEAT = 180;
	// Position of current cell
	int posx = global.p[idx].cellx;
	int posy = global.p[idx].celly;
	updateCellInfo(idx);
	// Calculate possible locations
	int next_row = posy + global.p[idx].vel[1];
	if(next_row >= g.max_rows)
		return;
	int prev_row = posy - global.p[idx].vel[1];
	if(prev_row < 0)
		prev_row = posy;
	int next_col = posx + global.p[idx].vel[0];
	if(next_col >= g.max_cols)
		next_col = posx;
	int prev_col = posx - global.p[idx].vel[0];
	if(prev_col < 0)
		prev_col = posx;

	// Fire interactions heat up metal
	if(g.cells[prev_row][posx].id == FIRE && g.cells[prev_row][posx].taken) {
		if(global.p[idx].color[0] < MAX_HEAT)
			global.p[idx].color[0] += 5;
	} else if(g.cells[next_row][posx].id == FIRE && g.cells[next_row][posx].taken) {
		if(global.p[idx].color[0] < MAX_HEAT)
			global.p[idx].color[0] += 5;
	} else if(g.cells[posy][next_col].id == FIRE && g.cells[posy][next_col].taken) {
		if(global.p[idx].color[0] < MAX_HEAT)
			global.p[idx].color[0] += 5;
	} else if(g.cells[posy][prev_col].id == FIRE && g.cells[posy][prev_col].taken) {
		if(global.p[idx].color[0] < MAX_HEAT)
			global.p[idx].color[0] += 5;
	}

	// Water interactions cool metal
	if(g.cells[prev_row][posx].id == WATER && g.cells[prev_row][posx].taken) {
		if(global.p[idx].color[0] > MIN_HEAT) {
			global.p[idx].color[0] -= 5;
			int idx1 = g.cells[prev_row][posx].idx;
			global.p[idx1].id = STEAM;
			global.p[idx1].reinit();
		}
	} else if(g.cells[next_row][posx].id == WATER && g.cells[next_row][posx].taken) {
		if(global.p[idx].color[0] > MIN_HEAT) {
			global.p[idx].color[0] -= 5;
			int idx1 = g.cells[next_row][posx].idx;
			global.p[idx1].id = STEAM;
			global.p[idx1].reinit();
		}
	} else if(g.cells[posy][next_col].id == WATER && g.cells[posy][next_col].taken) {
		if(global.p[idx].color[0] > MIN_HEAT) {
			global.p[idx].color[0] -= 5;
			int idx1 = g.cells[posy][next_col].idx;
			global.p[idx1].id = STEAM;
			global.p[idx1].reinit();
		}
	} else if(g.cells[posy][prev_col].id == WATER && g.cells[posy][prev_col].taken) {
		if(global.p[idx].color[0] > MIN_HEAT) {
			global.p[idx].color[0] -= 5;
			int idx1 = g.cells[posy][prev_col].idx;
			global.p[idx1].id = STEAM;
			global.p[idx1].reinit();
		}
	}
}

// End ********************

void physics()
{
	static auto time_since_burn = std::chrono::high_resolution_clock::now();
	for(int i=0; i<global.pcount; i++) {
		// Particle movement & interactions
		switch(global.p[i].id) {
			case SAND:
				updateSand(i);
				break;
			case WATER:
				updateWater(i);
				break;
			case WOOD:
				updateWood(i);
				break;
			case FIRE: {
				// Don't let fire burn instantly
				auto now = std::chrono::high_resolution_clock::now();
				std::chrono::duration<double, std::milli> elapsed = now - time_since_burn;
				if(elapsed.count() > SPREADTIME) {
					updateFire(i);
					time_since_burn = std::chrono::high_resolution_clock::now();
				}
				break;
				   }
			case SMOKE:
				updateSmoke(i);
				break;
			case STEAM:
				updateSteam(i);
				break;
			case SALT:
				updateSalt(i);
				break;
			case METAL:
				updateMetal(i);
				break;
		}
	}
	// Delete particles with the doKill set true
	deallocateParticles();
}

void mainMenu()
{
	// Displays menu which allows choosing between blocks
	s.setColor(0, 200, 150);
	s.fillRect(0, 0, global.xres, 100);
	int x = 0, y = 0, w = 20, h = 20;
	int offsetx = 25, offsety = 25;
	int textSize = 25;

	// Temp particle
	Particle p;
	p.initParticle(0, 0, 0, 0, 0, -1);
	for(int i=0; i<NUM_PARTICLES; i++) {
		p.id = i;
		p.reinit();
		if(global.select == i) {
			// Highlight current option
			s.setColor(0, 240, 150);
			s.fillRect(x, y, (2*offsetx), (2*offsety));
		}
		if(global.mousex >= x && global.mousex < x+(2*offsetx) &&
				global.mousey >= y && global.mousey < 100) {
			// Highlight new option
			s.setColor(0, 240, 150);
			s.fillRect(x, y, (2*offsetx), (2*offsety));
			if(global.lbutton_down) {
				// Choose this particle
				global.select = i;
			}
		}
		s.setColor(p.color[0], p.color[1], p.color[2]);
		s.fillRect(x+offsetx, y+offsety, w, h);
		s.setColor(0, 0, 0);
		// Menu options
		switch(i) {
			case SAND:
				s.drawText(x, y+(offsety/2), w, h, "SAND", textSize, i);
				break;
			case WATER:
				s.drawText(x, y+(offsety/2), w, h, "WATER", textSize, i);
				break;
			case WOOD:
				s.drawText(x, y+(offsety/2), w, h, "WOOD", textSize, i);
				break;
			case FIRE:
				s.drawText(x, y+(offsety/2), w, h, "FIRE", textSize, i);
				break;
			case SMOKE:
				s.drawText(x, y+(offsety/2), w, h, "SMOKE", textSize, i);
				break;
			case STEAM:
				s.drawText(x, y+(offsety/2), w, h, "STEAM", textSize, i);
				break;
			case SALT:
				s.drawText(x, y+(offsety/2), w, h, "SALT", textSize, i);
				break;
			case METAL:
				s.drawText(x, y+(offsety/2), w, h, "METAL", textSize, i);
				break;
		}
		x += 2*offsetx;
	}
}

void render()
{
	/*
	// Draw grid
	s.setColor(255, 255, 255);
	for(int i=0; i<g.max_rows; i++) {
		for(int j=0; j<g.max_cols; j++) {
			s.drawLine(g.cells[i][j].x, g.cells[i][j].y,
					g.cells[i][j].x+g.cells[i][j].w,
					g.cells[i][j].y);
			s.drawLine(g.cells[i][j].x, g.cells[i][j].y,
					g.cells[i][j].x,
					g.cells[i][j].y+g.cells[i][j].h);
		}
	}
	*/

	// Draw particles
	for(int i=0; i<global.pcount; i++) {
		s.setColor(global.p[i].color[0],global.p[i].color[1], global.p[i].color[2]);
		int w = g.cells[global.p[i].celly][global.p[i].cellx].w;
		int h = g.cells[global.p[i].celly][global.p[i].cellx].h;
		s.fillRect(global.p[i].x, global.p[i].y, w, h);
	}

	// Menu overlaps particles
	if(global.show_menu)
		mainMenu();
}

int main()
{
	// SDL
	if(s.init())
		exit(EXIT_FAILURE);
	bool quit = false;
	
	g.initGrid(GRIDSIZEX, GRIDSIZEY, global.xres, global.yres);
	
	// Calculating FPS
	int frame_count = 0;
	auto one_sec = std::chrono::high_resolution_clock::now();
	auto clock = std::chrono::high_resolution_clock::now();

	while(!quit) {
		frame_count++;
		// All input
		quit = s.handleInput();
		// Main program display
		if(!global.pause) {
			// Continually generate/delete particles if mouse button is held
			if(global.lbutton_down)
				generateParticles();
			if(global.rbutton_down)
				deleteParticles();
			s.preRender();
			render();
			s.postRender();
			physics();
		}

		// Top-center flushing out particles
		if(global.flush) {
			int half = g.max_cols / 2;
			float x = g.cells[0][half].x;
			float y = g.cells[0][half].y;
			allocateParticle(global.select, x, y, half, 0);
		}

		// FPS count
		auto now = std::chrono::high_resolution_clock::now();
		std::chrono::duration<float, std::milli> update = now-one_sec;

		if(update.count() >= 1000) {
			// Restart the sec
			one_sec = std::chrono::high_resolution_clock::now();
			
			now = std::chrono::high_resolution_clock::now();
			std::chrono::duration<float, std::milli> elapsed = now-clock;
			
			printf("\rFPS: %.2f", frame_count / (elapsed.count() / 1000.0f));
			fflush(stdout);
		}
	}

	return 0;
}
