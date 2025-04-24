#pragma once

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include<vector>
#include "imgui.h"
#include "backends/imgui_impl_sdl3.h"
#include "backends/imgui_impl_sdlrenderer3.h"


class Game
{
public:
    Game(int width, int height);
    ~Game();

    bool init(const char *title);
    void handleEvents();
    void render();
    void update();

    bool running() const { return isRunning; }
    void clean();

    bool showGrid = true;
private:
    int WINDOW_W;
    int WINDOW_H;
    bool isRunning;

    SDL_Window *window = nullptr;
    SDL_Renderer *renderer = nullptr;
};