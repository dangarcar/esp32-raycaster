#include <iostream>
#include <thread>
#include <cmath>

#include "SDL2/SDL.h"
#include "SDL2/SDL_main.h"

#include "../src/game/game.hpp"

#define SENSITIVITY 2

constexpr int FPS = 18; //To look more like Arduino 

constexpr int W = 960;
constexpr int H = 640;

SDL_Window* window;
SDL_Renderer* renderer;
SDL_Event event;

void render() {
    Color background = Color(fogColor);
    SDL_SetRenderDrawColor(renderer, background.r, background.g, background.b, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer);
    
    for(int y=0; y<Screen::SCREEN_HEIGHT; ++y) {
        for(int x=0; x<Screen::SCREEN_WIDTH; ++x) {
            Color pixel = Color(Screen::_screen[y*Screen::SCREEN_WIDTH + x]);
            SDL_SetRenderDrawColor(renderer, pixel.r, pixel.g, pixel.b, SDL_ALPHA_OPAQUE);
            
            struct SDL_Rect rect;
            rect.x = x*4; rect.y = y*4;
            rect.h = rect.w = 4;
            SDL_RenderFillRect(renderer, &rect);
        }
    }

    SDL_RenderPresent(renderer);
}

int main(int argc, char *argv[]) {

    SDL_Init(SDL_INIT_EVERYTHING);

    window = SDL_CreateWindow("Arduino Raycaster Port", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, W, H, SDL_WINDOW_ALLOW_HIGHDPI);
    renderer = SDL_CreateRenderer(window, -1, 0);
    SDL_SetRelativeMouseMode(SDL_TRUE);

    if(window == nullptr) {
        std::cerr << "SDL failed!\n";
        return -1;
    }

    Screen::fillScreen(screen_start);
    render();
    Game::setup();
    std::this_thread::sleep_for(std::chrono::seconds(1));

    float delta = 0.0;
    bool over = false;
    while(true) {
        auto t = getTime();
        InputData input {0, 0, 0, true};

        while(SDL_PollEvent(&event)) {
            switch (event.type){
            case SDL_QUIT:
                goto end;
            case SDL_MOUSEMOTION:
                input.x = event.motion.xrel * SENSITIVITY;
            break;
            case SDL_MOUSEBUTTONDOWN:
                if(event.button.button == SDL_BUTTON_LEFT)
                    input.flags |= (1 << 7);
                else if(event.button.button == SDL_BUTTON_RIGHT)
                    input.flags |= (1 << 6);
                break;
            case SDL_KEYDOWN:
                if(event.key.keysym.scancode == SDL_SCANCODE_ESCAPE)
                    goto end;
            default: 
                break;
            }
        }

        auto kb = SDL_GetKeyboardState(NULL);
        input.flags |= kb[SDL_SCANCODE_W]? (1 << UP) : 0;
        input.flags |= kb[SDL_SCANCODE_A]? (1 << LEFT) : 0;
        input.flags |= kb[SDL_SCANCODE_S]? (1 << DOWN) : 0;
        input.flags |= kb[SDL_SCANCODE_D]? (1 << RIGHT) : 0;

        if(input.rightClick() || input.leftClick()) {
            if(over) 
                Game::reset(&input);
            over = false;
        }

        if(!over) over = Game::loop(delta, input);

        render();

        do {
            delta = (getTime() - t)/1000000.0f;
        } while(delta < 1.0/FPS);
        //std::cout << 1/delta << '\n';
    }

end:

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}