#define _USE_MATH_DEFINES

#include <cmath>
#include <iostream>

#include "SDL.h"
#include "renderer.h"
#include "utils.h"

int main()
{
    frameBuffer buffer;
    buffer.width = 1024;
    buffer.height = 512;
    buffer.clear(pack_color(255, 255, 255));

    depthBuffer depth;
    depth.width = 512;
    depth.height = 512;

    player mainPlayer{3.456f, 2.345f, 1.523f, M_PI / 3.0};

    map gameMap;

    texture wallTextures("./walltext.png");
    texture monsterTextures("./monsters.png");
    if (!wallTextures.count || !monsterTextures.count)
    {
        std::cerr << "FAILED TO LOAD TEXTURES\n";
        return -1;
    }
    std::vector<sprite> sprites{{3.523, 3.812, 2}, {1.834, 8.765, 0}, {5.323, 5.365, 1}, {4.123, 10.265, 1}}; // gridPositions
    render(buffer, depth, gameMap, mainPlayer, sprites, wallTextures, monsterTextures);

    SDL_Window *window = nullptr;
    SDL_Renderer *renderer = nullptr;

    if (!SDL_Init(SDL_INIT_VIDEO)) // SDL3 returns true on success, unlike SDL2's 0-on-success
    {
        std::cerr << "Couldn't initialize SDL: " << SDL_GetError() << std::endl;
        return -1;
    }

    if (!SDL_CreateWindowAndRenderer("MINI RAYCASTER", buffer.width, buffer.height, SDL_WINDOW_INPUT_FOCUS, &window, &renderer))
    {
        std::cerr << "Couldn't create window and renderer: " << SDL_GetError() << std::endl;
        return -1;
    }

    SDL_Texture *framebuffer_texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ABGR8888, SDL_TEXTUREACCESS_STREAMING, buffer.width, buffer.height);
    SDL_UpdateTexture(framebuffer_texture, NULL, reinterpret_cast<void *>(buffer.data.data()), buffer.width * 4);

    bool running = true;
    while (running)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event)) // drain the whole queue each frame, else it backs up
        {
            if (event.type == SDL_EVENT_QUIT)
                running = false;
        }

        SDL_RenderClear(renderer);
        SDL_RenderTexture(renderer, framebuffer_texture, NULL, NULL);
        SDL_RenderPresent(renderer);
    }

    SDL_DestroyTexture(framebuffer_texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    SDL_Quit();
    
    // drop_ppm_image("./out.ppm", buffer.data, buffer.width, buffer.height);

    return 0;
}
