#include "/opt/homebrew/include/SDL2/SDL.h"
#include "/opt/homebrew/include/SDL2/SDL_ttf.h"
#include <iostream>
#include <string>

const int WIDTH = 800, HEIGHT = 600;
const int PADDLE_WIDTH = 20, PADDLE_HEIGHT = 100;
const int BALL_SIZE = 20;
const int PADDLE_SPEED = 10;
const int BALL_SPEED = 5;
const Uint32 COUNTDOWN_DURATION = 3000;

struct Paddle {
    SDL_Rect rect;
};

struct Ball {
    SDL_Rect rect;
    int dx = BALL_SPEED, dy = BALL_SPEED;
};

SDL_Texture* renderText(SDL_Renderer* renderer, TTF_Font* font, const std::string& text, SDL_Color color, SDL_Rect& boxOut) {
    SDL_Surface* surface = TTF_RenderText_Solid(font, text.c_str(), color);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    boxOut = {0, 0, surface->w, surface->h};
    SDL_FreeSurface(surface);
    return texture;
}

void movePaddle(Paddle& paddle, int dy) {
    paddle.rect.y += dy;
    if (paddle.rect.y < 0) paddle.rect.y = 0;
    if (paddle.rect.y > HEIGHT - PADDLE_HEIGHT) paddle.rect.y = HEIGHT - PADDLE_HEIGHT;
}

void resetBall(Ball& ball, int dxDirection, bool& waiting, Uint32& countdownStart) {
    ball.rect.x = WIDTH / 2 - BALL_SIZE / 2;
    ball.rect.y = HEIGHT / 2 - BALL_SIZE / 2;
    ball.dx = dxDirection * BALL_SPEED;
    ball.dy = BALL_SPEED;
    waiting = true;
    countdownStart = SDL_GetTicks();
}

void moveBall(Ball& ball, Paddle& p1, Paddle& p2, int& score1, int& score2, bool& waiting, Uint32& countdownStart) {
    ball.rect.x += ball.dx;
    ball.rect.y += ball.dy;

    if (ball.rect.y <= 0 || ball.rect.y >= HEIGHT - BALL_SIZE)
        ball.dy = -ball.dy;

    if (SDL_HasIntersection(&ball.rect, &p1.rect) || SDL_HasIntersection(&ball.rect, &p2.rect))
        ball.dx = -ball.dx;

    if (ball.rect.x <= 0) {
        score2++;
        resetBall(ball, 1, waiting, countdownStart);
    }

    if (ball.rect.x >= WIDTH) {
        score1++;
        resetBall(ball, -1, waiting, countdownStart);
    }
}

int main() {
    SDL_Init(SDL_INIT_VIDEO);
    TTF_Init();

    SDL_Window* win = SDL_CreateWindow("Pong with Restart Button", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, 0);
    SDL_Renderer* ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);
    TTF_Font* font = TTF_OpenFont("arial.ttf", 32);
    SDL_Event e;

    
    bool welcomeShown = false;
    while (!welcomeShown) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                SDL_Quit();
                return 0;
            }
            if (e.type == SDL_KEYDOWN) {
                welcomeShown = true;
            }
        }

        SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
        SDL_RenderClear(ren);

        SDL_Color white = {255, 255, 255, 255};
        SDL_Color red = {255, 0, 0, 255};
        SDL_Rect box;

    SDL_Rect box1;
    SDL_Texture* welcomeText = renderText(ren, font, "Welcome to Pong!", white, box1);
    box1.x = WIDTH / 2 - box1.w / 2;
    box1.y = HEIGHT / 2 - box1.h;

    SDL_SetRenderDrawColor(ren, red.r, red.g, red.b, red.a);
    SDL_RenderFillRect(ren, &box1);
    SDL_RenderCopy(ren, welcomeText, NULL, &box1);
    SDL_DestroyTexture(welcomeText);

    SDL_Rect box2;
    SDL_Texture* spacebarText = renderText(ren, font, "Press Spacebar to Begin", white, box2);
    box2.x = WIDTH / 2 - box2.w / 2;
    box2.y = box1.y + box1.h + 20;

    SDL_SetRenderDrawColor(ren, red.r, red.g, red.b, red.a);
    SDL_RenderFillRect(ren, &box2);
    SDL_RenderCopy(ren, spacebarText, NULL, &box2);
    SDL_DestroyTexture(spacebarText);


        SDL_RenderPresent(ren);
        SDL_Delay(16);
    }


    Ball ball;
    Paddle p1, p2;

    SDL_Color white = {255, 255, 255, 255};
    SDL_Color red = {255, 0, 0, 255};
    SDL_Color blue = {0, 150, 255, 255};

    bool running = true;
    bool paused = false;
    bool waiting = false;
    bool gameStarted = false;
    bool modeSelected = false;
    bool isAI = false;
    bool aiHardMode = false;

    int score1 = 0, score2 = 0;
    Uint32 countdownStart = 0;
    

    SDL_Rect restartBtn = {0, 0, 0, 0};  // Will be updated dynamically

    auto resetGame = [&]() {
        score1 = 0;
        score2 = 0;
        ball.rect = {WIDTH / 2 - BALL_SIZE / 2, HEIGHT / 2 - BALL_SIZE / 2, BALL_SIZE, BALL_SIZE};
        p1.rect = {50, HEIGHT / 2 - PADDLE_HEIGHT / 2, PADDLE_WIDTH, PADDLE_HEIGHT};
        p2.rect = {WIDTH - 70, HEIGHT / 2 - PADDLE_HEIGHT / 2, PADDLE_WIDTH, PADDLE_HEIGHT};
        paused = false;
        waiting = false;
        gameStarted = false;
        modeSelected = false;
    };

    resetGame();

    while (running) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) running = false;

            if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT) {
                int mx = e.button.x;
                int my = e.button.y;
                if (mx >= restartBtn.x && mx <= restartBtn.x + restartBtn.w &&
                    my >= restartBtn.y && my <= restartBtn.y + restartBtn.h) {
                    resetGame();
                }
            }

            if (e.type == SDL_KEYDOWN) {
                if (!modeSelected) {
                    switch (e.key.keysym.sym) {
                        case SDLK_1: isAI = false; modeSelected = true; break;
                        case SDLK_2: isAI = true; aiHardMode = false; modeSelected = true; break;
                        case SDLK_3: isAI = true; aiHardMode = true; modeSelected = true; break;
                    }
                } else {
                    switch (e.key.keysym.sym) {
                        case SDLK_p: paused = !paused; break;
                        case SDLK_SPACE:
                            if (!gameStarted) {
                                gameStarted = true;
                                waiting = true;
                                countdownStart = SDL_GetTicks();
                            } else if (!waiting) {
                                waiting = true;
                                countdownStart = SDL_GetTicks();
                            }
                            break;
                        case SDLK_r: resetGame(); break;
                    }
                }
            }
        }

        const Uint8* keystate = SDL_GetKeyboardState(NULL);
        if (modeSelected && gameStarted && !waiting && !paused) {
            if (keystate[SDL_SCANCODE_W]) movePaddle(p1, -PADDLE_SPEED);
            if (keystate[SDL_SCANCODE_S]) movePaddle(p1, PADDLE_SPEED);

            if (isAI) {
                int aiSpeed = aiHardMode ? PADDLE_SPEED - 1 : 3;
                int aiCenter = p2.rect.y + PADDLE_HEIGHT / 2;
                int ballCenter = ball.rect.y + BALL_SIZE / 2;
                if (aiCenter < ballCenter - 10) movePaddle(p2, aiSpeed);
                else if (aiCenter > ballCenter + 10) movePaddle(p2, -aiSpeed);
            } else {
                if (keystate[SDL_SCANCODE_UP]) movePaddle(p2, -PADDLE_SPEED);
                if (keystate[SDL_SCANCODE_DOWN]) movePaddle(p2, PADDLE_SPEED);
            }
        }

        if (gameStarted && !paused && !waiting)
            moveBall(ball, p1, p2, score1, score2, waiting, countdownStart);

        if (waiting && SDL_GetTicks() - countdownStart >= COUNTDOWN_DURATION)
            waiting = false;

        // Rendering
        SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
        SDL_RenderClear(ren);

        SDL_SetRenderDrawColor(ren, 255, 255, 255, 255);
        SDL_RenderFillRect(ren, &p1.rect);
        SDL_RenderFillRect(ren, &p2.rect);
        SDL_RenderFillRect(ren, &ball.rect);

        if (isAI) {
            SDL_Rect labelBox;
            SDL_Texture* youText = renderText(ren, font, "You", white, labelBox);
            labelBox.x = p1.rect.x + PADDLE_WIDTH / 2 - labelBox.w / 2;
            labelBox.y = p1.rect.y - labelBox.h - 10;
            SDL_RenderCopy(ren, youText, NULL, &labelBox);
            SDL_DestroyTexture(youText);
        }

        if (!modeSelected) {
            SDL_Rect box;
            SDL_Texture* modeText = renderText(ren, font, "1: 2P  2: AI Easy  3: AI Hard", white, box);
            box.x = WIDTH / 2 - box.w / 2;
            box.y = HEIGHT / 2 - box.h / 2;
            SDL_SetRenderDrawColor(ren, red.r, red.g, red.b, red.a);
            SDL_RenderFillRect(ren, &box);
            SDL_RenderCopy(ren, modeText, NULL, &box);
            SDL_DestroyTexture(modeText);
            SDL_RenderPresent(ren);
            SDL_Delay(16);
            continue;
        }

        if (!gameStarted) {
            SDL_Rect box;
            SDL_Texture* startText = renderText(ren, font, "Press SPACE to Start", white, box);
            box.x = WIDTH / 2 - box.w / 2;
            box.y = HEIGHT / 2 - box.h / 2;

            SDL_SetRenderDrawColor(ren, red.r, red.g, red.b, red.a);
            SDL_RenderFillRect(ren, &box);
            SDL_RenderCopy(ren, startText, NULL, &box);
            SDL_DestroyTexture(startText);
        }

        if (gameStarted) {
            SDL_Rect scoreBox;
            SDL_Texture* scoreText = renderText(ren, font, std::to_string(score1) + " : " + std::to_string(score2), white, scoreBox);
            scoreBox.x = WIDTH / 2 - scoreBox.w / 2;
            scoreBox.y = 20;
            SDL_RenderCopy(ren, scoreText, NULL, &scoreBox);
            SDL_DestroyTexture(scoreText);

            SDL_Texture* restartText = renderText(ren, font, "Restart", white, restartBtn);
            restartBtn.x = scoreBox.x + scoreBox.w + 20;
            restartBtn.y = scoreBox.y;
            SDL_SetRenderDrawColor(ren, 255, 0, 0, 255);
            SDL_RenderFillRect(ren, &restartBtn);
            SDL_RenderCopy(ren, restartText, NULL, &restartBtn);
            SDL_DestroyTexture(restartText);
        }

        if (waiting) {
            Uint32 timeLeft = 3 - ((SDL_GetTicks() - countdownStart) / 1000);
            if (timeLeft > 0) {
                SDL_Rect box;
                SDL_Texture* countText = renderText(ren, font, std::to_string(timeLeft), white, box);
                box.x = WIDTH / 2 - box.w / 2;
                box.y = HEIGHT / 2 - box.h / 2;
                SDL_SetRenderDrawColor(ren, red.r, red.g, red.b, red.a);
                SDL_RenderFillRect(ren, &box);
                SDL_RenderCopy(ren, countText, NULL, &box);
                SDL_DestroyTexture(countText);
            }
        }

        if (paused && gameStarted) {
            SDL_Rect box;
            SDL_Texture* pauseText = renderText(ren, font, "PAUSED", white, box);
            box.x = WIDTH / 2 - box.w / 2;
            box.y = HEIGHT / 2 + 50;
            SDL_RenderCopy(ren, pauseText, NULL, &box);
            SDL_DestroyTexture(pauseText);
        }

        SDL_RenderPresent(ren);
        SDL_Delay(16);
    }

    TTF_CloseFont(font);
    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    TTF_Quit();
    SDL_Quit();
    return 0;
}
