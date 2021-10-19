/* ===== *\
|* Setup *|
\* ===== */
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <iostream>
#include <string>
using namespace std;

/* Initialize SDL2 */
SDL_Renderer *renderer;
SDL_Window *window;
TTF_Font *font;
SDL_Color color;

/* Variables */
int frameCount;
int lastFrame;
int timerFPS;
int fps;
bool running = true;
/* Globals */
#define WIDTH 720
#define HEIGHT 720
#define FONT_SIZE 32
#define FONT_PATH "assets/Peepo.ttf"

/* Player and paddles */
SDL_Rect scoreBoard;
SDL_Rect leftPaddle;
int leftScore;
SDL_Rect rightPaddle;
int rightScore;
SDL_Rect ball; /* TODO: turn into a circle */
float velX;
float velY;
string score;
bool leftPlayerTurn;

/* ======================= *\
|* Draw text to scoreboard *|
\* ======================= */
int drawTextToScoreboard(string text, int x, int y) {
    SDL_Surface *surface;
    SDL_Texture *texture;

    /* Ensure font is loaded */
    if (font == NULL) {
        cerr << "Attempted to draw text, but font is null";
        return 1;
    }

    /* Create surface and texture */
    const char *textString = text.c_str();
    surface = TTF_RenderText_Solid(font, textString, color);
    texture = SDL_CreateTextureFromSurface(renderer, surface);

    /* Update scoreboard */
    scoreBoard.w = surface->w;
    scoreBoard.h = surface->h;
    scoreBoard.x = x - scoreBoard.w;
    scoreBoard.y = y - scoreBoard.h;

    /* We don't have a rect here (not needed), hence the NULL */
    SDL_RenderCopy(renderer, texture, NULL, &scoreBoard);

    /* Cleanup */
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);

    return 0;
}

/* ====== *\
|* Update *|
\* ====== */
void update(void) { return; }

/* ===== *\
|* Input *|
\* ===== */
void input(void) {
    /* Pull events */
    SDL_Event event;
    const Uint8 *keystates = SDL_GetKeyboardState(NULL);

    /* Events */
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) /* exit */
            running = false;
    }

    /* Keys */
    /* exit */
    if (keystates[SDL_SCANCODE_ESCAPE]) {
        running = false;
    }
    /* up */
    if (keystates[SDL_SCANCODE_UP]) {
        ;
    }
    /* down */
    if (keystates[SDL_SCANCODE_DOWN]) {
        ;
    }

    return;
}

/* ====== *\
|* Render *|
\* ====== */
void render(void) {
    SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 255);
    SDL_RenderClear(renderer); /* clear */

    /* Delay for stable FPS, like clock.tick */
    ++frameCount;
    timerFPS = SDL_GetTicks() - lastFrame;
    if (timerFPS < (1000 / 60)) {
        SDL_Delay((1000 / 60) - timerFPS);
    }

    SDL_SetRenderDrawColor(renderer, color.r, color.b, color.g, 255);
    SDL_RenderPresent(renderer);

    return;
}

/* ==== *\
|* Main *|
\* ==== */
int main(void) {
    /* Create window with error handling */
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        cerr << "Failed to initialize SDL" << endl;
        return 1;
    }
    if (SDL_CreateWindowAndRenderer(WIDTH, HEIGHT, 0, &window, &renderer) < 0) {
        cerr << "Failed to create window" << endl;
        return 1;
    }
    static int lastTime = 0;
    /* Intialize other stuffs */
    TTF_Init();
    font = TTF_OpenFont(FONT_PATH, FONT_SIZE);

    /* Color */
    color.r = color.g = color.b = 255;

    /* Main loop */
    do {
        /* Frame and FPS stuff */
        lastFrame = SDL_GetTicks();
        if (lastFrame >= (lastTime + 1000)) { /* A second has passed */
            lastTime = lastFrame;
            fps = frameCount;
            frameCount = 0;
        }

        /* Game code */
        update();
        input();
        render();
    } while (running);

    return 0;
}