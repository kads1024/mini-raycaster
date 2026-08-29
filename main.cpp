#define _USE_MATH_DEFINES

#include <cmath>
#include <cstdint>
#include <iostream>
#include <vector>

#include "SDL.h"
#include "renderer.h"
#include "utils.h"

namespace
{
constexpr size_t windowWidth = 1024;
constexpr size_t windowHeight = 512;

/// The first-person view occupies the right half of the window, so the depth
/// buffer only has to cover that half.
constexpr size_t viewportWidth = windowWidth / 2;

/// Radians turned and grid units walked per frame.
constexpr double playerTurnSpeed = .05;
constexpr double playerWalkSpeed = .1;

/// @brief Translates one key event into the movement state held on the player.
void apply_input(const SDL_Event &event, player &mainPlayer)
{
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

/// @brief Advances the player by one frame, refusing moves that end inside a wall.
void update_player(player &mainPlayer, const map &gameMap)
{
    mainPlayer.viewDirectionAngle += float(mainPlayer.turn) * playerTurnSpeed;
    float nx = mainPlayer.x + mainPlayer.walk * std::cos(mainPlayer.viewDirectionAngle) * playerWalkSpeed;
    float ny = mainPlayer.y + mainPlayer.walk * std::sin(mainPlayer.viewDirectionAngle) * playerWalkSpeed;

    if (int(nx) >= 0 && int(nx) < int(gameMap.width) && int(ny) >= 0 && int(ny) < int(gameMap.height) && gameMap.is_cell_empty(nx, ny))
    {
        mainPlayer.x = nx;
        mainPlayer.y = ny;
    }
}
} // namespace

int main()
{
    frameBuffer buffer(windowWidth, windowHeight, pack_color(255, 255, 255));
    depthBuffer depth(viewportWidth, windowHeight);

    player mainPlayer{3.456f, 2.345f, 1.523f, float(M_PI / 3.0), 0, 0};

    map gameMap;

    texture wallTextures("./walltext.png");
    texture monsterTextures("./monsters.png");
    if (!wallTextures.count || !monsterTextures.count)
    {
        std::cerr << "FAILED TO LOAD TEXTURES\n";
        return -1;
    }

    std::vector<sprite> sprites{{3.523, 3.812, 2}, {1.834, 8.765, 0}, {5.323, 5.365, 1}, {4.123, 10.765, 1}}; // gridPositions

    if (!SDL_Init(SDL_INIT_VIDEO)) // SDL3 returns true on success, unlike SDL2's 0-on-success
    {
        std::cerr << "Couldn't initialize SDL: " << SDL_GetError() << std::endl;
        return -1;
    }

    SDL_Window *window = nullptr;
    SDL_Renderer *renderer = nullptr;

    if (!SDL_CreateWindowAndRenderer("MINI RAYCASTER", static_cast<int>(buffer.width), static_cast<int>(buffer.height), SDL_WINDOW_INPUT_FOCUS, &window, &renderer))
    {
        std::cerr << "Couldn't create window and renderer: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return -1;
    }

    SDL_Texture *framebufferTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ABGR8888, SDL_TEXTUREACCESS_STREAMING, static_cast<int>(buffer.width), static_cast<int>(buffer.height));
    if (!framebufferTexture)
    {
        std::cerr << "Couldn't create the framebuffer texture: " << SDL_GetError() << std::endl;
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }

    SDL_Event event;
    while (true)
    {
        if (SDL_PollEvent(&event))
        {
            if (SDL_EVENT_QUIT == event.type || (SDL_EVENT_KEY_DOWN == event.type && SDLK_ESCAPE == event.key.key))
                break;

            apply_input(event, mainPlayer);
        }

        update_player(mainPlayer, gameMap);

        render(buffer, depth, gameMap, mainPlayer, sprites, wallTextures, monsterTextures);

        SDL_UpdateTexture(framebufferTexture, nullptr, buffer.data.data(), static_cast<int>(buffer.width * sizeof(uint32_t)));

        SDL_RenderClear(renderer);
        SDL_RenderTexture(renderer, framebufferTexture, nullptr, nullptr);
        SDL_RenderPresent(renderer);
    }

    SDL_DestroyTexture(framebufferTexture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    SDL_Quit();

    return 0;
}
