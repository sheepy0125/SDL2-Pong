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

/* ====== *\
|* Update *|
\* ====== */
void update(void) {
    ;
    return;
}

/* ===== *\
|* Input *|
\* ===== */
void input(void) {
    ;
    return;
}

/* ====== *\
|* Render *|
\* ====== */
void render(void) {
    /* Delay for stable FPS, like clock.tick */
    ++frameCount;
    timerFPS = SDL_GetTicks() - lastFrame;
    if (timerFPS < (1000 / 60)) {
        SDL_Delay((1000 / 60) - timerFPS);
    }
    return;
}

/* ==== *\
|* Main *|
\* ==== */
int main(void) {
    /* Create window with error handling */
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        cout << "Failed to initialize SDL" << endl;
        return 1;
    }
    if (SDL_CreateWindowAndRenderer(WIDTH, HEIGHT, 0, &window, &renderer) < 0) {
        cout << "Failed to create window" << endl;
        return 1;
    }
    static int lastTime = 0;
    /* Intialize other stuffs */
    TTF_Init();
    font = TTF_OpenFont(FONT_PATH, FONT_SIZE);

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