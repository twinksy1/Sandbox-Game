#include "display.h"

Display::Display()
{
    window = NULL;
    screen_surface = NULL;
    rend = NULL;
    for(int i=0; i<NUM_PARTICLES; i++) {
        message[i] = NULL;
        blockTextures[i] = NULL;
    }
    //Initialize PNG loading
    int imgFlags = IMG_INIT_PNG;
    if( !( IMG_Init( imgFlags ) & imgFlags ) )
    {
        printf( "SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError() );
    }
}
Display::~Display()
{
    SDL_FreeSurface(screen_surface);
    for(int i=0; i<NUM_PARTICLES; i++) {
        if(blockTextures[i] != NULL)
            SDL_DestroyTexture(blockTextures[i]);
        SDL_DestroyTexture(message[i]);
    }
    SDL_DestroyWindow(window);
    SDL_Quit();
    printf("\nSuccessful exit\n");
}
int Display::init()
{
    if(SDL_Init(SDL_INIT_VIDEO) == -1) {
        printf("ERROR SDL_INIT: %s\n", SDL_GetError());
        return 1;
    }

    window = SDL_CreateWindow("Sandbox Simulation", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
            xres, yres, SDL_WINDOW_SHOWN);
    if(window == NULL) {
        printf("ERROR DISPLAYING WINDOW: %s\n", SDL_GetError());
        return 1;
    }

    rend = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if(rend == NULL) {
        printf("ERROR RENDERER: %s\n", SDL_GetError());
        return 1;
    }
    loadTextures();
    TTF_Init();
    
    return 0;
}
void Display::setColor(int r, int g, int b)
{
    SDL_SetRenderDrawColor(rend, r, g, b, 255);
}
void Display::preRender()
{
    // Rendering function
    //screen_surface = SDL_GetWindowSurface(window);
    //SDL_FillRect(screen_surface, NULL, SDL_MapRGB(screen_surface->format, WHITE[0], WHITE[1], WHITE[2]));
    //SDL_UpdateWindowSurface(window);

    // Clear screen
    setColor(0, 0, 0);
    SDL_RenderClear(rend);
}
void Display::postRender()
{
    // Show renderer
    SDL_RenderPresent(rend);
}
// Drawing functions
void Display::drawLine(int x1, int y1, int x2, int y2)
{
    SDL_RenderDrawLine(rend, x1, y1, x2, y2);
}
void Display::drawPoint(int x, int y)
{
    SDL_RenderDrawPoint(rend, x, y);
}
void Display::drawRect(int x, int y, int w, int h)
{
    SDL_Rect rect;
    rect.x = x;
    rect.y = y;
    rect.w = w;
    rect.h = h;
    SDL_RenderDrawRect(rend, &rect);
}
void Display::fillRect(int x, int y, int w, int h)
{
    SDL_Rect rect;
    rect.x = x;
    rect.y = y;
    rect.w = w;
    rect.h = h;
    SDL_RenderFillRect(rend, &rect);
}
void Display::loadTextures()
{
    for(int i=0; i<NUM_PARTICLES; i++) {
        switch (i)
        {
        case SAND:
            blockTextures[i] = IMG_LoadTexture(rend, "./Blocks/sand/tile0.png");
            break;
        case WATER:
            blockTextures[i] = IMG_LoadTexture(rend, "./Blocks/water/tile0.png");
            break;
        case FIRE:
            blockTextures[i] = IMG_LoadTexture(rend, "./Blocks/fire/tile0.png");
            break;
        case WOOD:
            blockTextures[i] = IMG_LoadTexture(rend, "./Blocks/wood/tile0.png");
            break;
        case SMOKE:
            blockTextures[i] = IMG_LoadTexture(rend, "./Blocks/smoke/tile0.png");
            break;
        case STEAM:
            blockTextures[i] = IMG_LoadTexture(rend, "./Blocks/steam/tile0.png");
            break;
        default:
            break;
        }
    }
}
void Display::updateTextures()
{

    const int FIRE_SPRITE_NUM = 2;
    const int WATER_SPRITE_NUM = 3;
    const int SMOKE_SPRITE_NUM = 4;
    const int STEAM_SPRITE_NUM = 4;

    static int update = 0;

    // How often to change between textures, i.e. Water movement, fire movement, etc.
    static int waterNum = 0;
    static int fireNum = 0;
    static int smokeNum = 0;
    static int steamNum = 0;
    
    const std::string waterFile = "./Blocks/water/tile" + std::to_string(waterNum) + ".png";
    const std::string fireFile = "./Blocks/fire/tile" + std::to_string(fireNum) + ".png";
    const std::string smokeFile = "./Blocks/smoke/tile" + std::to_string(smokeNum) + ".png";
    const std::string steamFile = "./Blocks/steam/tile" + std::to_string(steamNum) + ".png";
    
    blockTextures[WATER] = IMG_LoadTexture(rend, waterFile.c_str());
    blockTextures[FIRE] = IMG_LoadTexture(rend, fireFile.c_str());
    blockTextures[SMOKE] = IMG_LoadTexture(rend, smokeFile.c_str());
    blockTextures[STEAM] = IMG_LoadTexture(rend, steamFile.c_str());

    // Move to next sprite frame
    update++;
    if(update % 2 == 0) {
        fireNum++;
    }
    if(update % 5 == 0) {
        waterNum++;
        smokeNum++;
    }
    if(update >= 10) {
        steamNum++;
        update = 0;
    }

    // Safe guards
    if(waterNum >= WATER_SPRITE_NUM) {
        waterNum = 0;
    }
    if(smokeNum >= SMOKE_SPRITE_NUM) {
        smokeNum = 0;
    }
    if(steamNum >= STEAM_SPRITE_NUM) {
        steamNum = 0;
    }
    if(fireNum >= FIRE_SPRITE_NUM) {
        fireNum = 0;
    }
}
void Display::drawTexture(int x, int y, int id)
{
    SDL_Rect textureRect;
    textureRect.x = x;
    textureRect.y = y;
    textureRect.w = textureRect.h = P_SIZE;
    switch (id) {
    case SAND:
        SDL_RenderCopy(rend, blockTextures[SAND], NULL, &textureRect);
        break;
    case WATER:
        SDL_RenderCopy(rend, blockTextures[WATER], NULL, &textureRect);
        break;
    case FIRE:
        SDL_RenderCopy(rend, blockTextures[FIRE], NULL, &textureRect);
        break;
    case WOOD:
        SDL_RenderCopy(rend, blockTextures[WOOD], NULL, &textureRect);
        break;
    case SMOKE:
        SDL_RenderCopy(rend, blockTextures[SMOKE], NULL, &textureRect);
        break;
    case STEAM:
        SDL_RenderCopy(rend, blockTextures[STEAM], NULL, &textureRect);
        break;
    default:
        break;
    }
}
void Display::drawText(int x, int y, int w, int h, const char* mes, int size, int i)
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
    SDL_Rect MessageRect;
    MessageRect.x = x+extra;
    MessageRect.y = y+extra;
    MessageRect.w = w+extra;
    MessageRect.h = h;
    SDL_RenderCopy(rend, message[i], NULL, &MessageRect);
}

bool Display::handleInput(Game& game)
{
    SDL_Event e;
    // Handles all events in queue
    while(SDL_PollEvent(&e) != 0) {
        if(e.type == SDL_QUIT)
            return true;
        else if(e.type == SDL_KEYDOWN || e.type == SDL_KEYUP)
            if(keyboardInput(game, e))
                return true;
        // Always check mouse
        checkMouse(game, e);			
    }

    return false;
}