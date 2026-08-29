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

    player mainPlayer{3.456f, 2.345f, 1.523f, M_PI / 3.0, 0, 0};

    map gameMap;

    texture wallTextures("./walltext.png");
    texture monsterTextures("./monsters.png");
    if (!wallTextures.count || !monsterTextures.count)
    {
        std::cerr << "FAILED TO LOAD TEXTURES\n";
        return -1;
    }
    std::vector<sprite> sprites{{3.523, 3.812, 2}, {1.834, 8.765, 0}, {5.323, 5.365, 1}, {4.123, 10.765, 1}}; // gridPositions
    // render(buffer, depth, gameMap, mainPlayer, sprites, wallTextures, monsterTextures);

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
    // SDL_UpdateTexture(framebuffer_texture, NULL, reinterpret_cast<void *>(buffer.data.data()), buffer.width * 4);

    SDL_Event event;
    while (1)
    {
        if (SDL_PollEvent(&event))
        {
            if (SDL_EVENT_QUIT == event.type || (SDL_EVENT_KEY_DOWN == event.type && SDLK_ESCAPE == event.key.key))
                break;
            if (SDL_EVENT_KEY_UP == event.type)
            {
                if ('a' == event.key.key || 'd' == event.key.key)
                    mainPlayer.turn = 0;
                if ('w' == event.key.key || 's' == event.key.key)
                    mainPlayer.walk = 0;
            }
            if (SDL_EVENT_KEY_DOWN == event.type)
            {
                if ('a' == event.key.key)
                    mainPlayer.turn = -1;
                if ('d' == event.key.key)
                    mainPlayer.turn = 1;
                if ('w' == event.key.key)
                    mainPlayer.walk = 1;
                if ('s' == event.key.key)
                    mainPlayer.walk = -1;
            }
        }

        mainPlayer.viewDirectionAngle += float(mainPlayer.turn) * .05;
        float nx = mainPlayer.x + mainPlayer.walk * cos(mainPlayer.viewDirectionAngle) * .1;
        float ny = mainPlayer.y + mainPlayer.walk * sin(mainPlayer.viewDirectionAngle) * .1;

        if (int(nx) >= 0 && int(nx) < int(gameMap.width) && int(ny) >= 0 && int(ny) < int(gameMap.height) && gameMap.is_cell_empty(nx, ny))
        {
            mainPlayer.x = nx;
            mainPlayer.y = ny;
        }

        render(buffer, depth, gameMap, mainPlayer, sprites, wallTextures, monsterTextures);
        SDL_UpdateTexture(framebuffer_texture, NULL, reinterpret_cast<void *>(buffer.data.data()), buffer.width * 4);

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
