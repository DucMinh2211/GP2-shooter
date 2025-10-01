#include <SDL2/SDL.h>
#include <SDL2/SDL_error.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_video.h>
#include <cstdlib>
#include <iostream>
#include <vector>
#include "Constant.h"
#include "ResourceManager.h"
#include "components/inc/Character.h"
#include "components/inc/IRenderable.h"
#include "components/inc/IUpdatable.h"
#include "components/inc/InputHandler.h"
#include "components/inc/Bullet.h"
#include "math/Vector2.h"
#include "components/inc/BlackHole.h"
#include "components/inc/Circle.h"
#include "components/inc/OBB.h"
#include "components/inc/AnimatedSprite.h"



int main (int argc, char *argv[]) {
    // SDL_Init
    if (SDL_Init(SDL_INIT_VIDEO)) {
        std::cerr << "SDL_Init failed" << SDL_GetError();
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


    ResourceManager resource_manager(renderer);

    // Create a red texture for player 1
    SDL_Surface* red_surface = SDL_CreateRGBSurface(0, 16, 16, 32, 0, 0, 0, 0);
    SDL_FillRect(red_surface, NULL, SDL_MapRGB(red_surface->format, 255, 0, 0));
    SDL_Texture* red_texture = SDL_CreateTextureFromSurface(renderer, red_surface);
    SDL_FreeSurface(red_surface);
    if (red_texture == nullptr) {
        std::cerr << "Texture creation failed" << SDL_GetError();
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return EXIT_FAILURE;
    }

    // Create a black texture for player 2
    SDL_Surface* black_surface = SDL_CreateRGBSurface(0, 16, 16, 32, 0, 0, 0, 0);
    SDL_FillRect(black_surface, NULL, SDL_MapRGB(black_surface->format, 0, 0, 0));
    SDL_Texture* black_texture = SDL_CreateTextureFromSurface(renderer, black_surface);
    SDL_FreeSurface(black_surface);
    if (black_texture == nullptr) {
        std::cerr << "Texture creation failed" << SDL_GetError();
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return EXIT_FAILURE;
    }

    if (!resource_manager.load_texture("bullet", "assets/pictures/bulletA.png")) {
        std::cerr << "Failed to load bullet sprite!\n";
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        IMG_Quit();
        SDL_Quit();
        return EXIT_FAILURE;
    }

    // setup
    AnimatedSprite sprite(renderer, "assets/pictures/PlayerShooting.png", 24, 16, 5, 120); 
    Character player(Vector2(WINDOW_W / 2.0f - 50.0f, WINDOW_H / 2.0f), red_texture, 100.0f, 100.0f, true);
    Character player2(Vector2(WINDOW_W / 2.0f + 50.0f, WINDOW_H / 2.0f), black_texture, 100.0f, 100.0f, true);

    std::vector<IInputObject*> char_list;
    BlackHole blackhole(Vector2(WINDOW_W / 2.0f, WINDOW_H / 2.0f), 
                    120.0f, 60.0f, // outer radius, inner radius
                    5.0f, 15.0f);  // outer dps, inner dps

// tạo hitbox cho blackhole (chỉ để test)
    Circle* outer = new Circle(Vector2(WINDOW_W / 2.0f, WINDOW_H / 2.0f), 120.0f);
    Circle* inner = new Circle(Vector2(WINDOW_W / 2.0f, WINDOW_H / 2.0f), 60.0f);

    // thêm vào hitbox list
    blackhole.get_hitboxes().push_back(outer);
    blackhole.get_hitboxes().push_back(inner);

    InputHandler input_handler(InputSet::INPUT_1, player, char_list);
    InputHandler input_handler2(InputSet::INPUT_2, player2, char_list);

    std::vector<Bullet*> bullet_list;

    std::vector<IUpdatable*> updatable_list;
    updatable_list.push_back(&player);
    updatable_list.push_back(&input_handler);
    updatable_list.push_back(&player2);
    updatable_list.push_back(&input_handler2);

    // main loop
    bool running = true;
    SDL_Event event;
    Uint32 last_time = SDL_GetTicks();
    while (running) {
        while(SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
            else {
                input_handler.handle_event(event, bullet_list, resource_manager);
                input_handler2.handle_event(event, bullet_list, resource_manager);
            }
        }


        // Update game logic
        Uint32 current_time = SDL_GetTicks();
        float delta_time = (current_time - last_time) / 1000.0f;
        last_time = current_time;

        for (auto updatable : updatable_list) {
            updatable->update(delta_time);
        }

        for (Bullet* bullet : bullet_list) {
            bullet->update(delta_time);
        }
        for (size_t i = 0; i < bullet_list.size(); i++) {
            for (size_t j = i + 1; j < bullet_list.size(); j++) {
                bullet_list[i]->collide(*bullet_list[j]);
                bullet_list[j]->collide(*bullet_list[i]);
            }
        }
        for (size_t i = 0; i < bullet_list.size(); i++) {
            blackhole.collide(*bullet_list[i]);
        }


        // --- Rendering ---
        SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF); // Set draw color to white
        SDL_RenderClear(renderer);                            // Clear the screen

        // Render player 1
        Vector2 pos = player.get_position();
        const SDL_Texture* tex = player.get_sprite();
        if (tex) {
            SDL_Rect dstRect = { (int)pos.x - 8, (int)pos.y - 8, 16, 16 };
            SDL_RenderCopy(renderer, const_cast<SDL_Texture*>(tex), NULL, &dstRect);
        }

        // Render player 2
        Vector2 pos2 = player2.get_position();
        const SDL_Texture* tex2 = player2.get_sprite();
        if (tex2) {
            SDL_Rect dstRect2 = { (int)pos2.x - 8, (int)pos2.y - 8, 16, 16 };
            SDL_RenderCopy(renderer, const_cast<SDL_Texture*>(tex2), NULL, &dstRect2);
        }

        // render bullets
        for (Bullet* bullet : bullet_list) {
            IRenderable* obj = dynamic_cast<IRenderable*>(bullet);
            obj->render(renderer);
        }
        // render hitbox blackhole
        for (HitBox* hb : blackhole.get_hitboxes()) {
            hb->debug_draw(renderer, {0, 0, 255, 255}); // xanh dương để thấy rõ
        }


        SDL_RenderPresent(renderer);                          // Update the screen
    }

    resource_manager.unload_all();

    // Quit SDL
    SDL_DestroyTexture(red_texture);
    SDL_DestroyTexture(black_texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return EXIT_SUCCESS;
}
