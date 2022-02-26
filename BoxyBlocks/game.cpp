#include "game.h"

Camera::Camera()
{
    focusx = (float)xres / 2.0f;
    focusy = (float)yres / 2.0f;
}

Game::Game()
{

}
Game::~Game()
{
}

bool Game::Init() {
    return window.Init(xres, yres, title);
}

void Game::Render() {
    window.PreRender();

    window.PostRender();
}

void Game::displayCellInfo(float* pos)
{

}

void Game::pan(int dir)
{
    if(dir == -1) {

    } else {

    }
}

bool Game::HandleEvents()
{
    SDL_Event e;
    // Handles all events in queue
    while (SDL_PollEvent(&e) != 0) {
        if (e.type == SDL_QUIT)
            return true;
        else if (e.type == SDL_KEYDOWN || e.type == SDL_KEYUP)
            if (KeyboardEvent(e))
                return true;
        // Always check mouse
        MouseEvent(e);
    }

    return false;
}

bool Game::KeyboardEvent(SDL_Event& e) {
    static bool ignore = false;
    // Key up
    if (e.type == SDL_KEYUP) {
        switch (e.key.keysym.sym) {
        case SDLK_LSHIFT:
            shift_down = false;
            //printf("Shift key released\n");
            break;
        }
    }
    // Handle keyboard input
    switch (e.key.keysym.sym) {
    case SDLK_a:
        pan(1);
        break;
    case SDLK_d:
        pan(-1);
        break;
    case SDLK_m:
        if (!ignore)
            show_menu ^= 1;
        ignore ^= 1;
        break;
    case SDLK_p:
        if (!ignore)
            pause ^= 1;
        ignore ^= 1;
        break;
    case SDLK_F1:
        if (!ignore)
            debugMode ^= 1;
        ignore ^= 1;
        break;
    case SDLK_f:
        if (!ignore)
            flush ^= 1;
        ignore ^= 1;
        break;
    case SDLK_LSHIFT:
        if (!ignore)
            shift_down = true;
        ignore ^= 1;
        break;
    case SDLK_ESCAPE:
        return true;
        break;
    }
    return false;
}

void Game::MouseEvent(SDL_Event& e) {
    if (e.type == SDL_MOUSEMOTION) {
        // Mouse moved
        SDL_GetMouseState(&mousex, &mousey);
        /*
        if(game.mousex >=0 && game.mousex < game.xres &&
                game.mousey >= 0 && game.mousey < game.yres)
            game.inbounds = true;
        else game.inbounds = false;
        //printf("Mouse: %i, %i\n", mousex, mousey);
        */
    }

    if (e.type == SDL_MOUSEBUTTONDOWN) {
        // Mouse click
        // If shift down, display cell info
        if ((e.button.button == SDL_BUTTON_LEFT) && shift_down) {
            float mouse[2] = { (float)mousex, (float)mousey };
            displayCellInfo(mouse);
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
        else if (e.button.button == SDL_BUTTON_LEFT || lbutton_down) {
            lbutton_down = true;
            rbutton_down = false;
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
        else if (e.button.button == SDL_BUTTON_RIGHT || rbutton_down) {
            // Right click
            lbutton_down = false;
            rbutton_down = true;
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

    if (e.type == SDL_MOUSEBUTTONUP) {
        // Button release
        lbutton_down = false;
        rbutton_down = false;
    }
}