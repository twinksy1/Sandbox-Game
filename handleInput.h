#ifndef HANDLEINPUT_H
#define HANDLEINPUT_H

#include <SDL2/SDL.h>
#include "game.h"

inline bool keyboardInput(Game& game, SDL_Event e)
{
    static bool ignore = false;
    // Key up
    if(e.type == SDL_KEYUP) {
        switch(e.key.keysym.sym) {
            case SDLK_LSHIFT:
                game.shift_down = false;
                //printf("Shift key released\n");
                break;
        }
    }
    // Handle keyboard input
    switch(e.key.keysym.sym) {
        case SDLK_a:
            game.pan(1);
            break;
        case SDLK_d:
            game.pan(-1);
            break;
        case SDLK_0:
            game.select = SAND;
            break;
        case SDLK_1:
            game.select = WATER;
            break;
        case SDLK_2:
            game.select = WOOD;
            break;
        case SDLK_3:
            game.select = FIRE;
            break;
        case SDLK_4:
            game.select = SMOKE;
            break;
        case SDLK_5:
            game.select = STEAM;
            break;
        case SDLK_6:
            game.select = STONE;
            break;
        case SDLK_m: 
            if(!ignore)
                game.show_menu ^= 1;
            ignore ^= 1;
            break;
        case SDLK_p: 
            if(!ignore)
                game.pause ^= 1;
            ignore ^= 1;
            break;
        case SDLK_F1: 
            if(!ignore)
                game.debugMode ^= 1;
            ignore ^= 1;
            break;
        case SDLK_f: 
            if(!ignore)
                game.flush ^= 1;
            ignore ^= 1;
            break;
        case SDLK_LSHIFT:
            if(!ignore)
                game.shift_down = true;
            ignore ^= 1;
            break;
        case SDLK_ESCAPE:
            return true;
            break;
    }
    return false;
}
inline void checkMouse(Game& game, SDL_Event e)
{
    if(e.type == SDL_MOUSEMOTION) {
        // Mouse moved
        SDL_GetMouseState(&game.mousex, &game.mousey);
        /*
        if(game.mousex >=0 && game.mousex < game.xres && 
                game.mousey >= 0 && game.mousey < game.yres)
            game.inbounds = true;
        else game.inbounds = false;
        //printf("Mouse: %i, %i\n", mousex, mousey);
        */
    }

    if(e.type == SDL_MOUSEBUTTONDOWN) {
        // Mouse click
        // If shift down, display cell info
        if((e.button.button == SDL_BUTTON_LEFT) && game.shift_down) {
            float mouse[2] = {(float)game.mousex, (float)game.mousey};
            game.displayCellInfo(mouse);
            /*
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
            game.p[g.cells[celly][cellx].idx].printInfo();
            */
        }

        // Else generate a particle
        else if(e.button.button == SDL_BUTTON_LEFT || game.lbutton_down) {
            game.lbutton_down = true;
            game.rbutton_down = false;
            /*
            if(game.show_menu && game.mousey < 100) return;
            // Left click, generate particle at closest cell
            float mouse[2] = {(float)game.mousex, (float)game.mousey};
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
            if(closest_dist < game.radius) {
                allocateParticle(game.select,
                            g.cells[celly_idx][cellx_idx].x,
                        g.cells[celly_idx][cellx_idx].y,	cellx_idx, celly_idx);
            }
            */
        }
        else if(e.button.button == SDL_BUTTON_RIGHT || game.rbutton_down) {
            // Right click
            game.lbutton_down = false;
            game.rbutton_down = true;
            /*
            float mouse[2] = {(float)game.mousex, (float)game.mousey};
            for(int i=0; i<game.pcount; i++) {
                float pos[2] = {game.p[i].x, game.p[i].y};
                float dist = getLength(pos, mouse);
                if(dist < game.radius)
                    game.p[i].doKill = true;
            }

            deallocateParticles();
            */
        }
    }

    if(e.type == SDL_MOUSEBUTTONUP) {
        // Button release
        game.lbutton_down = false;
        game.rbutton_down = false;
    }
}

#endif