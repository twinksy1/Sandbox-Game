#include "game.h"
#include <Windows.h>
#include <WinUser.h>
#include <cstdlib>

Camera::Camera()
{
    focusx = (float)xres / 2.0f;
    focusy = (float)yres / 2.0f;
}

Game::Game() {
    
}
Game::~Game() {
    for (auto font : fontMap) {
        TTF_CloseFont(font.second);
    }
}

bool Game::Init() {
    if (window.Init(xres, yres, title)) {
        error = Errors::SDL_WINDOW;
        return -1;
    }

    std::string fontPath = std::string(std::getenv("FONTS")) + "/Roboto-Black.ttf";
    TTF_Font* font = TTF_OpenFont(fontPath.c_str(), 24);
    if (font == NULL) {
        std::string ttfError = std::string(TTF_GetError());
        std::wstring errorMessage = L"Font Load Error: " + std::wstring(ttfError.begin(), ttfError.end());
        MessageBox(NULL, (LPCWSTR)errorMessage.c_str(), (LPCWSTR)L"SDL TTF", MB_OK);
        error = Errors::FONT_LOAD;
        return -1;
    }

    fontMap.emplace("Roboto-Black-24", font);

    entities.push_back(Entity(std::pair<double, double>(0.0, 0.0), std::pair<double, double>(20.0, 20.0), std::pair<double, double>(1.0, 1.0)));
    entities.push_back(Entity(std::pair<double, double>(100.0, 100.0), std::pair<double, double>(20.0, 20.0), std::pair<double, double>(0.0, 0.0)));
    entities.push_back(Entity(std::pair<double, double>(80.0, 300.0), std::pair<double, double>(20.0, 20.0), std::pair<double, double>(0.0, 0.0)));
    return 0;
}

void Game::Render() {
    window.PreRender();

#ifdef _DEBUG
    std::string mouseCoords = std::to_string(mousex) + ", " + std::to_string(mousey);
    window.DrawScreenText(mousex, mousey, mouseCoords.length() * 5, mouseCoords.length() * 5, fontMap["Roboto-Black-24"], mouseCoords);
#endif // DEBUG

    window.SetColor(255, 255, 0);
    for (auto& entity : entities) {
        window.FillRect(entity.GetPos(), entity.GetDimmensions());
    }

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

void Game::CheckForCollisions() {
    for (int i = 0; i < entities.size(); i++) {
        for (int j = 0; j < entities.size(); j++) {
            if (j == i) { continue; }
            else {
                entities[i].CheckCollision(&entities[j]);
            }
        }
    }
}

void Game::MoveEntities() {
    if (gameFreeze) {
        return;
    }
    for (auto& entity : entities) {
        entity.Move();
    }
}

bool Game::Run() {
    frameStart = SDL_GetTicks();
    Render();
    MoveEntities();
    CheckForCollisions();
    bool eventReturn = HandleEvents();
    frameTime = SDL_GetTicks() - frameStart;
    if (frameTime < 10/targetFrameRate*10) {
        SDL_Delay(10/targetFrameRate*10 - frameTime);
    }
    return !eventReturn;
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
    }

    if (e.type == SDL_MOUSEBUTTONDOWN) {

        if (e.button.button == SDL_BUTTON_LEFT || lbutton_down) {
            lbutton_down = true;
            rbutton_down = false;

        }
        else if (e.button.button == SDL_BUTTON_RIGHT || rbutton_down) {
            // Right click
            lbutton_down = false;
            rbutton_down = true;

        }
    }

    if (e.type == SDL_MOUSEBUTTONUP) {
        // Button release
        lbutton_down = false;
        rbutton_down = false;
    }
}