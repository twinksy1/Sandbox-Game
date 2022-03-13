#include "display.h"
#include <Windows.h>
#include <WinUser.h>
#include "Line.h"
#include <locale>
#include <codecvt>

Display::Display()
{
    window = NULL;
    screenSurface = NULL;
    rend = NULL;
    /*for(int i=0; i<NUM_PARTICLES; i++) {
        message[i] = NULL;
        blockTextures[i] = NULL;
    }*/
    //Initialize PNG loading
    int imgFlags = IMG_INIT_PNG;
    if( !( IMG_Init( imgFlags ) & imgFlags ) ) {
        std::string error = "SDL_image Error: " + std::string(IMG_GetError());
        std::wstring errorMessage(error.begin(), error.end());
        MessageBox(NULL, (LPCWSTR)errorMessage.c_str(), (LPCWSTR)L"SDL IMAGE COULD NOT INITIALIZE!", MB_OK);
    }
}
Display::~Display()
{
    SDL_FreeSurface(screenSurface);
    /*for(int i=0; i<NUM_PARTICLES; i++) {
        if(blockTextures[i] != NULL)
            SDL_DestroyTexture(blockTextures[i]);
        SDL_DestroyTexture(message[i]);
    }*/
    SDL_DestroyWindow(window);
    SDL_Quit();
}
int Display::Init(int xres, int yres, std::string title)
{
    if(SDL_Init(SDL_INIT_VIDEO) == -1) {
        std::string error = "ERROR SDL_INIT: " + std::string(SDL_GetError());
        std::wstring errorMessage(error.begin(), error.end());
        MessageBox(NULL, (LPCWSTR)errorMessage.c_str(), (LPCWSTR)"SDL COULD NOT INITIALIZE!", MB_OK);
        return 1;
    }

    window = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
            xres, yres, SDL_WINDOW_SHOWN);
    if(window == NULL) {
        std::string error = "ERROR DISPLAYING WINDOW: " + std::string(SDL_GetError());
        std::wstring errorMessage(error.begin(), error.end());
        MessageBox(NULL, (LPCWSTR)errorMessage.c_str(), (LPCWSTR)"SDL WINDOW COULD NOT INITIALIZE!", MB_OK);
        return 1;
    }

    rend = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if(rend == NULL) {
        std::string error = "ERROR RENDERER: " + std::string(SDL_GetError());
        std::wstring errorMessage(error.begin(), error.end());
        MessageBox(NULL, (LPCWSTR)errorMessage.c_str(), (LPCWSTR)"SDL RENDERER COULD NOT INITIALIZE!", MB_OK);
        return 1;
    }
    LoadTextures();
    if (TTF_Init()) {
        std::string error = "TTF ERROR: " + std::string(TTF_GetError());
        std::wstring errorMessage(error.begin(), error.end());
        MessageBox(NULL, (LPCWSTR)errorMessage.c_str(), (LPCWSTR)"SDL TTF COULD NOT INITIALIZE!", MB_OK);
        return 1;
    }
    
    return 0;
}

std::wstring Display::ExePath() {
    TCHAR buffer[MAX_PATH] = { 0 };
    GetModuleFileName(NULL, buffer, MAX_PATH);
    std::wstring::size_type pos = std::wstring(buffer).find_last_of(L"\\/");
    return std::wstring(buffer).substr(0, pos);
}

void Display::SetColor(int r, int g, int b) {
    currentColor = 0;
    currentColor += r;
    currentColor << 8;
    currentColor += g;
    currentColor << 8;
    currentColor += b;
    SDL_SetRenderDrawColor(rend, r, g, b, 255);
}

void Display::SetColor(SDL_Color color) {
    SDL_SetRenderDrawColor(rend, color.r, color.g, color.b, 255);
}

void Display::PreRender() {
    // Clear screen
    SetColor(0, 0, 0);
    SDL_RenderClear(rend);
}

void Display::PostRender() {
    // Show renderer
    SDL_RenderPresent(rend);
}

// Drawing functions
void Display::DrawLine(int x1, int y1, int x2, int y2) {
    SDL_RenderDrawLine(rend, x1, y1, x2, y2);
}

void Display::DrawLine(Point p1, Point p2) {
    SDL_RenderDrawLineF(rend, p1.x, p1.y, p2.x, p2.y);
}

void Display::DrawLine(Line line) {
    SDL_RenderDrawLineF(rend, line.GetPt1()->x, line.GetPt1()->y, line.GetPt2()->x, line.GetPt2()->y);
}

void Display::DrawPoint(int x, int y) {
    SDL_RenderDrawPoint(rend, x, y);
}

void Display::DrawPoint(Point pt) {
    DrawPoint(pt.x, pt.y);
}

SDL_FRect Display::CreateRectF(double x, double y, double w, double h) {
    return CreateRectF((float)x, (float)y, (float)w, (float)h);
}

SDL_Rect Display::CreateRect(int x, int y, int w, int h) {
    SDL_Rect rect;
    rect.x = x;
    rect.y = y;
    rect.w = w;
    rect.h = h;
    return rect;
}

SDL_Rect Display::CreateRect(float x, float y, float w, float h) {
    SDL_Rect rect;
    rect.x = x;
    rect.y = y;
    rect.w = w;
    rect.h = h;
    return rect;
}

SDL_FRect Display::CreateRectF(float x, float y, float w, float h) {
    SDL_FRect rect;
    rect.x = x;
    rect.y = y;
    rect.w = w;
    rect.h = h;
    return rect;
}

void Display::DrawRect(int x, int y, int w, int h) {
    SDL_Rect rect = CreateRect(x, y, w, h);
    SDL_RenderDrawRect(rend, &rect);
}

void Display::DrawRect(float x, float y, float w, float h) {
    SDL_FRect rect = CreateRectF(x, y, w, h);
    SDL_RenderDrawRectF(rend, &rect);
}

void Display::DrawRect(double x, double y, double w, double h) {
    DrawRect((float)x, (float)y, (float)w, (float)h);
}

void Display::DrawRect(std::pair<float, float> pos, std::pair<float, float> dimmensions) {
    DrawRect(pos.first, pos.second, dimmensions.first, dimmensions.second);
}

void Display::DrawRect(std::pair<double, double> pos, std::pair<double, double> dimmensions) {
    DrawRect(pos.first, pos.second, dimmensions.first, dimmensions.second);
}

void Display::DrawRect(Square square) {
    std::shared_ptr<Point> pt = square.GetUpperLeftPoint();
    std::pair<double, double> dimmensions = square.GetDimmensions();
    DrawRect(pt->x, pt->y, dimmensions.first, dimmensions.second);
}

void Display::FillRect(Square square) {
    std::shared_ptr<Point> pt = square.GetUpperLeftPoint();
    std::pair<double, double> dimmensions = square.GetDimmensions();
    FillRect(pt->x, pt->y, dimmensions.first, dimmensions.second);
}

void Display::FillRect(float x, float y, float w, float h) {
    SDL_FRect rect = CreateRectF(x, y, w, h);
    SDL_RenderFillRectF(rend, &rect);
    //SDL_FillRect(screenSurface, &rect, currentColor);
}

void Display::FillRect(double x, double y, double w, double h) {
    FillRect((float)x, (float)y, (float)w, (float)h);
}

void Display::FillRect(int x, int y, int w, int h) {
    SDL_Rect rect = CreateRect(x, y, w, h);
    SDL_RenderFillRect(rend, &rect);
}

void Display::FillRect(std::pair<float, float> pos, std::pair<float, float> dimmensions) {
    FillRect(pos.first, pos.second, dimmensions.first, dimmensions.second);
}

void Display::FillRect(std::pair<double, double> pos, std::pair<double, double> dimmensions) {
    FillRect(pos.first, pos.second, dimmensions.first, dimmensions.second);
}

void Display::FillRect(Point pt, std::pair<double, double> dimmensions) {
    FillRect(pt.x, pt.y, dimmensions.first, dimmensions.second);
}

void Display::FillRect(XY pt, XY dimmensions) {
    SDL_FRect rect = CreateRectF(pt.x, pt.y, dimmensions.x, dimmensions.y);
    SDL_RenderFillRectF(rend, &rect);
}

void Display::FillRect(Point pt, XY dimmensions) {
    SDL_FRect rect = CreateRectF(pt.x, pt.y, dimmensions.x, dimmensions.y);
    SDL_RenderFillRectF(rend, &rect);
}

void Display::LoadTextures()
{
    //for(int i=0; i<NUM_PARTICLES; i++) {
    //    std::string block = BLOCK_TYPES[i];
    //    int strlen = block.length();
    //    for(int j=0; j<strlen; j++) {
    //        block[j] = block[j] - ('Z' - 'z');
    //    }
    //    std::string filepath = "./Blocks/" + block + "/tile0.png";
    //    //printf("%s\n", filepath.c_str());
    //    blockTextures[i] = IMG_LoadTexture(rend, filepath.c_str());
    //}
    //bg = IMG_LoadTexture(rend, "./background.jpg");
}
void Display::UpdateTextures()
{

    //const int FIRE_SPRITE_NUM = 2;
    //const int WATER_SPRITE_NUM = 3;
    //const int SMOKE_SPRITE_NUM = 4;
    //const int STEAM_SPRITE_NUM = 4;

    //static int update = 0;

    //// How often to change between textures, i.e. Water movement, fire movement, etc.
    //static int waterNum = 0;
    //static int fireNum = 0;
    //static int smokeNum = 0;
    //static int steamNum = 0;
    //
    //const std::string waterFile = "./Blocks/water/tile" + std::to_string(waterNum) + ".png";
    //const std::string fireFile = "./Blocks/fire/tile" + std::to_string(fireNum) + ".png";
    //const std::string smokeFile = "./Blocks/smoke/tile" + std::to_string(smokeNum) + ".png";
    //const std::string steamFile = "./Blocks/steam/tile" + std::to_string(steamNum) + ".png";
    //
    ///*blockTextures[WATER] = IMG_LoadTexture(rend, waterFile.c_str());
    //blockTextures[FIRE] = IMG_LoadTexture(rend, fireFile.c_str());
    //blockTextures[SMOKE] = IMG_LoadTexture(rend, smokeFile.c_str());
    //blockTextures[STEAM] = IMG_LoadTexture(rend, steamFile.c_str());*/

    //// Move to next sprite frame
    //update++;
    //if(update % 2 == 0) {
    //    fireNum++;
    //}
    //if(update % 5 == 0) {
    //    waterNum++;
    //    smokeNum++;
    //}
    //if(update >= 10) {
    //    steamNum++;
    //    update = 0;
    //}

    //// Safe guards
    //if(waterNum >= WATER_SPRITE_NUM) {
    //    waterNum = 0;
    //}
    //if(smokeNum >= SMOKE_SPRITE_NUM) {
    //    smokeNum = 0;
    //}
    //if(steamNum >= STEAM_SPRITE_NUM) {
    //    steamNum = 0;
    //}
    //if(fireNum >= FIRE_SPRITE_NUM) {
    //    fireNum = 0;
    //}
}
void Display::DrawTexture(int x, int y, int id)
{
    //SDL_Rect textureRect;
    //textureRect.x = x;
    //textureRect.y = y;
    //textureRect.w = textureRect.h = P_SIZE;
    //SDL_RenderCopy(rend, blockTextures[id], NULL, &textureRect);
}
void Display::DrawBackground(int x, int y, int w, int h)
{
    SDL_Rect textureRect;
    textureRect.x = x;
    textureRect.y = y;
    textureRect.w = w;
    textureRect.h = h;
    //SDL_RenderCopy(rend, bg, NULL, &textureRect);
}
void Display::DrawScreenText(int x, int y, int w, int h, TTF_Font* font, std::string text, SDL_Color color) {
    try {
        if (font == NULL) return;
        SDL_Surface* surfaceMessage = TTF_RenderText_Solid(font, text.c_str(), color);
        SDL_Texture* messageTexture = SDL_CreateTextureFromSurface(rend, surfaceMessage);
        SDL_Rect messageRect = CreateRect(x, y, w, h);
        SDL_RenderCopy(rend, messageTexture, NULL, &messageRect);
        SDL_FreeSurface(surfaceMessage);
        SDL_DestroyTexture(messageTexture);
    }
    catch (...) {
        return;
    }
}

void Display::DrawScreenText(float x, float y, float w, float h, TTF_Font* font, std::string text, SDL_Color color) {
    try {
        if (font == NULL) return;
        SDL_Surface* surfaceMessage = TTF_RenderText_Solid(font, text.c_str(), color);
        SDL_Texture* messageTexture = SDL_CreateTextureFromSurface(rend, surfaceMessage);
        SDL_FRect messageRect = CreateRectF(x, y, w, h);
        SDL_RenderCopyF(rend, messageTexture, NULL, &messageRect);
        SDL_FreeSurface(surfaceMessage);
        SDL_DestroyTexture(messageTexture);
    }
    catch (...) {
        return;
    }
}

void Display::DrawScreenText(double x, double y, double w, double h, TTF_Font* font, std::string text, SDL_Color color) {
    DrawScreenText((float)x, (float)y, (float)w, (float)h, font, text, color);
}

void Display::DrawScreenText(std::pair<float, float> pos, std::pair<float, float> dimmensions, TTF_Font* font, std::string text, SDL_Color color) {
    DrawScreenText(pos.first, pos.second, dimmensions.first, dimmensions.second, font, text, color);
}

void Display::DrawScreenText(std::pair<double, double> pos, std::pair<double, double> dimmensions, TTF_Font* font, std::string text, SDL_Color color) {
    DrawScreenText(pos.first, pos.second, dimmensions.first, dimmensions.second, font, text, color);
}
