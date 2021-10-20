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
#define PI 3.14159265358979323846
#define WIDTH 720
#define HEIGHT 720
#define FONT_SIZE 32
#define FONT_PATH "assets/Peepo.ttf"
#define BALL_SIZE 16
#define BALL_START_SPEED 8
#define BALL_MAX_SPEED 16
#define BALL_SPEED_INC 1
#define PADDLE_SPEED 16
#define PADDLE_WIDTH 12
#define PADDLE_HEIGHT (HEIGHT / 8)
#define PADDLE_OFFSET 32
#define COMPUTER_PADDLE_SPEED (PADDLE_SPEED / 4)

/* Ball and paddles */
SDL_Rect scoreBoard;
SDL_Rect leftPaddle;
int leftScore;
SDL_Rect rightPaddle;
int rightScore;
SDL_Rect ball; /* TODO: turn into a circle */
int ballSpeed = BALL_START_SPEED;
float ballVelX, ballVelY = ballSpeed;
string score;
bool leftPlayerTurn;

/* ======================= *\
|* Turn bool into -1 or +1 *|
\* ======================= */
int turnPositiveOrNegative(bool theBool) {
    if (theBool)
        return 1;
    return -1;
}

/* ======================= *\
|* Draw text to scoreboard *|
\* ======================= */
int drawTextToScoreboard(string text) {
    int x = ((WIDTH / 2) + FONT_SIZE);
    int y = (FONT_SIZE * 2);

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

/* ===== *\
|* Serve *|
\* ===== */
void serve(void) {
    /* Reset paddles*/
    leftPaddle.y = rightPaddle.y = (HEIGHT / 2) - (PADDLE_HEIGHT / 2);

    /* Reset ball */
    ballSpeed = BALL_START_SPEED;
    ball.x = (WIDTH / 2) - (BALL_SIZE / 2);
    ball.y = (HEIGHT / 2) - (BALL_SIZE / 2);
    ballVelY = 0;

    /* Ball velocity */
    if (leftPlayerTurn)
        ballVelX = ballSpeed;
    else
        ballVelX = -ballSpeed;

    /* Switch turn (whoever's turn currently is the person who won */
    leftPlayerTurn = !leftPlayerTurn;

    return;
}

/* ============= *\
|* Ball movement *|
\* ============= */
void ballMovement(void) {
    ball.x += ballVelX;
    ball.y += ballVelY;
    return;
}

/* ================= *\
|* Computer movement *|
\* ================= */
void computerMovement(void) {
    /* Go to the ball */
    /* Need to go higher */
    if (ball.y < (rightPaddle.y + (PADDLE_HEIGHT / 2)))
        rightPaddle.y -= COMPUTER_PADDLE_SPEED;

    /* Need to go lower */
    if (ball.y > (rightPaddle.y + (PADDLE_HEIGHT / 2)))
        rightPaddle.y += COMPUTER_PADDLE_SPEED;

    return;
}

/* ================== *\
|* Get opposite angle *|
\* ================== */
double getOppositeAngle(int paddleY) {
    /* Gets the opposite angle of where the ball hit the paddle */

    /* What Y value on the paddle is the ball touching (from 0-PADDLE_HEIGHT) */
    double rel_y = (paddleY + (PADDLE_HEIGHT / 2)) - (ball.y + (BALL_SIZE / 2));
    /* Turn the relative Y value to be a number from 0-1 or 1-2, with the
     * turning point being halfway through the paddle */
    double normalized_y = rel_y / (PADDLE_HEIGHT / 2);
    /* Turn that into an angle using glorious π
     * The maximum angle will be 75° */
    double angle = normalized_y * (5 * PI / 12);

    return angle;
}

/* ======================= *\
|* Ball collision checking *|
\* ======================= */
void ballCollisionCheck(void) {
    /* Top or bottom */
    if (ball.y <= 0 || (ball.y + BALL_SIZE) >= HEIGHT)
        ballVelY *= -1;
    /* Off screen */
    else if ((ball.x - BALL_SIZE) > WIDTH) { /* left scored */
        serve();
        leftScore++;
    } else if (ball.x < 0) { /* right scored */
        serve();
        rightScore++;
    }

    double oppositeAngle;

    /* Touching paddles */
    if (SDL_HasIntersection(&ball, &leftPaddle)) {
        oppositeAngle = getOppositeAngle(leftPaddle.y);
        leftPlayerTurn = false;
    } else if (SDL_HasIntersection(&ball, &rightPaddle)) {
        oppositeAngle = -getOppositeAngle(rightPaddle.y);
        leftPlayerTurn = true;
    } else
        return;

    /* Touched one of the paddles */
    ballSpeed += BALL_SPEED_INC;
    if (ballSpeed > BALL_MAX_SPEED)
        ballSpeed = BALL_MAX_SPEED;

    /* Velocities */
    ballVelX = turnPositiveOrNegative(!leftPlayerTurn) * ballSpeed *
               cos(oppositeAngle);
    ballVelY = (sin(oppositeAngle) * ballSpeed);
    // ballVelY = (ballSpeed - abs(ballVelX)); /* Too slow, see updated */

    return;
}

/* ====== *\
|* Update *|
\* ====== */
void update(void) {
    /* Score */
    score = to_string(leftScore) + "   " + to_string(rightScore);

    /* Disallow paddles moving past the screen */
    /* Left paddle */
    if ((leftPaddle.y + PADDLE_HEIGHT) >= HEIGHT)
        leftPaddle.y = (HEIGHT - PADDLE_HEIGHT);
    else if (leftPaddle.y <= 0)
        leftPaddle.y = 0;
    /* Right paddle */
    if ((rightPaddle.y + PADDLE_HEIGHT) >= HEIGHT)
        rightPaddle.y = (HEIGHT - PADDLE_HEIGHT);
    else if (rightPaddle.y <= 0)
        rightPaddle.y = 0;

    /* Only move computer paddle when it's their turn */
    if (!leftPlayerTurn)
        computerMovement();

    ballMovement();
    ballCollisionCheck();

    return;
}

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
        leftPaddle.y -= PADDLE_SPEED;
    }
    /* down */
    if (keystates[SDL_SCANCODE_DOWN]) {
        leftPaddle.y += PADDLE_SPEED;
    }

    return;
}

/* ====== *\
|* Render *|
\* ====== */
void render(void) {
    SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 255);
    SDL_RenderClear(renderer);
    SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 255);

    /* Delay for stable FPS, like clock.tick */
    ++frameCount;
    timerFPS = SDL_GetTicks() - lastFrame;
    if (timerFPS < (1000 / 60)) {
        SDL_Delay((1000 / 60) - timerFPS);
    }

    /* Paddles and ball */
    SDL_RenderFillRect(renderer, &leftPaddle);
    SDL_RenderFillRect(renderer, &rightPaddle);
    SDL_RenderFillRect(renderer, &ball);

    /* Scoreboard */
    drawTextToScoreboard(score);

    /* Show everything */
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

    /* Ball and paddles */
    leftScore = rightScore = 0;
    leftPaddle.x = PADDLE_OFFSET;
    leftPaddle.h = PADDLE_HEIGHT;
    leftPaddle.y = (HEIGHT / 2) - (PADDLE_HEIGHT / 2); /* serve(); */
    leftPaddle.w = PADDLE_WIDTH;
    rightPaddle = leftPaddle; /* Right paddle is almost the same as
                                 the left paddle except for position */
    rightPaddle.x = (WIDTH - rightPaddle.w - 32);
    ball.w = ball.h = BALL_SIZE;

    /* Serve ball for first time */
    serve();

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
        input();
        update();
        render();
    } while (running);

    return 0;
}