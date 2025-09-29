#include <SDL2/SDL.h>
#include <SDL2/SDL_error.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_video.h>
#include <SDL2/SDL_image.h>
#include "ResourceManager.h"
#include <cstdlib>
#include <iostream>
#include "Constant.h"
#include <cmath>
#include <vector>

int main (int argc, char *argv[]) {
    // SDL_Init
    if (SDL_Init(SDL_INIT_VIDEO)) {
        std::cerr << "SDL_Init failed" << SDL_GetError();
        return EXIT_FAILURE;
    }

    // Initialize SDL_image
    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        std::cerr << "IMG_Init failed: " << IMG_GetError();
        SDL_Quit();
        return EXIT_FAILURE;
    }

    // Init Window
    SDL_Window* window = SDL_CreateWindow(GAME_TITLE, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_W, WINDOW_H,SDL_WINDOW_SHOWN );
    if (window == nullptr) {
        std::cerr << "Window Init failed" << SDL_GetError();
        SDL_Quit();
        return EXIT_FAILURE;
    }

    // Init Renderer
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == nullptr) {
        std::cerr << "Renderer Init failed" << SDL_GetError();
        SDL_DestroyWindow(window);
        SDL_Quit();
        return EXIT_FAILURE;
    }

    // Create resource manager
    ResourceManager resourceManager(renderer);
    // Load bullet texture only
    if (!resourceManager.load_texture("bullet", "assets/pictures/bulletA.png")) {
        std::cerr << "Failed to load bullet sprite!\n";
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        IMG_Quit();
        SDL_Quit();
        return EXIT_FAILURE;
    }

    // main loop
    bool running = true;
    SDL_Event event;
    while (running) {
        while(SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
        }

     // --- Add rendering commands here ---
    SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0x00); // Set draw color to white
    SDL_RenderClear(renderer);                            // Clear the screen

    // Example: render all bullets (replace with your bullet list/logic)
    struct DummyBullet { float x, y, dir_x, dir_y; };
    std::vector<DummyBullet> bullets = {
        {100, 100, 1, 0},
        {200, 150, 0, 1},
        {300, 200, -1, 1}
    };
    SDL_Texture* bulletTexture = resourceManager.get_texture("bullet");
    SDL_Rect srcRect = {0, 0, 24, 24}; // bullet sprite in sheet
    for (const auto& b : bullets) {
        SDL_Rect bullet_rect = { static_cast<int>(b.x), static_cast<int>(b.y), 24, 24 };
        float angle = std::atan2(b.dir_y, b.dir_x) * 180.0f / 3.14159265f;
        SDL_RenderCopyEx(renderer, bulletTexture, &srcRect, &bullet_rect, angle, NULL, SDL_FLIP_NONE);
    }

    SDL_RenderPresent(renderer);                          // Update the screen
    }

    // Quit SDL
    // ResourceManager will clean up textures automatically
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return EXIT_SUCCESS;
}

