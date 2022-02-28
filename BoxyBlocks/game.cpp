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

const Entity Game::GetLeftWall() {
    Square collisionBox = Square(Line(0.0, 0.0, 0.0, (double)yres), 
                                Line(6.0, 0.0, 6.0, (double)yres));
    std::pair<double, double> pos(0.0, 0.0);
    const Entity entity(pos, collisionBox);
    return entity;
}

const Entity Game::GetRightWall() {
    Square collisionBox = Square(Line((double)xres - 6.0, 0.0, (double)xres - 6.0, yres), 
                                Line((double)xres, 0.0, (double)xres, (double)yres));
    std::pair<double, double> pos(0.0, (double)xres - 6.0);
    const Entity entity(pos, collisionBox);
    return entity;
}

const Entity Game::GetFloor() {
    Square collisionBox = Square(Line(0.0, (double)yres - 6.0, (double)xres, (double)yres - 6.0), 
                                Line(0.0, (double)yres, (double)xres, (double)yres), false);
    std::pair<double, double> pos(0.0, (double)yres - 6.0);
    const Entity entity(pos, collisionBox);
    return entity;
}

const Entity Game::CreateEntity(double posx, double posy, double x0, double y0, double x1, double y1, double x2, double y2, double x3, double y3) {
    Square collisionBox = Square(Line(x0, y0, x1, y1), Line(x2, y2, x3, y3));
    std::pair<double, double> pos(posx, posy);
    const Entity entity(pos, collisionBox);
    return entity;
}

const Entity Game::CreateEntity(double posx, double posy, double x0, double y0, double x1, double y1, double x2, double y2, double x3, double y3, double v0, double v1) {
    Square collisionBox = Square(Line(x0, y0, x1, y1), Line(x2, y2, x3, y3));
    std::pair<double, double> pos(posx, posy);
    std::pair<double, double> vel(v0, v1);
    const Entity entity(pos, collisionBox, vel);
    return entity;
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

    // tmp borders
    entities.push_back(GetLeftWall());
    //entities.push_back(GetRightWall());
    //entities.push_back(GetFloor());

    entities.push_back(CreateEntity(300.0, 300.0, 0.0, 0.0, 0.0, 20.0, 20.0, 20.0, 20.0, 20.0, -20.1, 0.2));

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

    //static double angle = 0.0;
    //Square square1;
    //square1.SetTwoEqualSidesTB(Line(Point(150.0, 150.0), Point(200.0, 200.0)), Line(Point(300.0, 150.0), Point(350.0, 200.0)));
    //Square square2;
    //square2.SetAllEqualSides(Line(Point(100.0, 220.0), Point(250.0, 250.0)), SquareSides::RIGHT);

    //square1.Rotate(angle);
    //square2.Rotate(2.0 * angle);

    //square1.CheckCollision((Shape*)&square2);

    //if (square1.colliding || square2.colliding) {
    //    window.SetColor(255, 0, 0);
    //}
    //else {
    //    window.SetColor(0, 255, 0);
    //}
    //
    //for (int i = 0; (SquareSides)i < SquareSides::NUM_SQUARE_SIDES; i++) {
    //    Line line = square1.GetLine((SquareSides)i);
    //    window.DrawLine(line.GetPt1().x, line.GetPt1().y, line.GetPt2().x, line.GetPt2().y);
    //    line = square2.GetLine((SquareSides)i);
    //    window.DrawLine(line.GetPt1().x, line.GetPt1().y, line.GetPt2().x, line.GetPt2().y);
    //}

    //Line line1(100.0, 100.0, 200.0, 200.0);
    //Line line2(200.0, 100.0, 50.0, 0.0);

    //line1.Rotate(angle);

    //if (line1.CheckLineIntersection(line2)) {
    //    window.SetColor(255, 0, 0);
    //}
    //else {
    //    window.SetColor(0, 255, 0);
    //}

    //window.DrawLine(line1.GetPt1().x, line1.GetPt1().y, line1.GetPt2().x, line1.GetPt2().y);
    //window.DrawLine(line2.GetPt1().x, line2.GetPt1().y, line2.GetPt2().x, line2.GetPt2().y);

    //angle += 0.01;

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
    //if (gameFreeze) {
    //    return;
    //}
    //for (auto& entity : entities) {
    //    if (!entity.Collided()) {
    //        entity.Move();
    //    }
    //}
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