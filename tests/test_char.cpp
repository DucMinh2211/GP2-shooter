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

    // Set the logical size for rendering to scale the world to the window
    SDL_RenderSetLogicalSize(renderer, WORLD_W, WORLD_H);


    ResourceManager resource_manager(renderer);

    // Create a transparent texture for team 1
    SDL_Surface* red_surface = SDL_CreateRGBSurface(0, 16, 16, 32,
        0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000); // RGBA masks
    SDL_FillRect(red_surface, NULL, SDL_MapRGBA(red_surface->format, 0, 0, 0, 0));
    SDL_Texture* red_texture = SDL_CreateTextureFromSurface(renderer, red_surface);
    SDL_FreeSurface(red_surface);
    SDL_SetTextureBlendMode(red_texture, SDL_BLENDMODE_BLEND); // cho phép alpha

    // Create a transparent texture for team 2
    SDL_Surface* blue_surface = SDL_CreateRGBSurface(0, 16, 16, 32,
        0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000); // RGBA masks
    SDL_FillRect(blue_surface, NULL, SDL_MapRGBA(blue_surface->format, 0, 0, 0, 0));
    SDL_Texture* blue_texture = SDL_CreateTextureFromSurface(renderer, blue_surface);
    SDL_FreeSurface(blue_surface);
    SDL_SetTextureBlendMode(blue_texture, SDL_BLENDMODE_BLEND);

    if (!resource_manager.load_texture("bullet", "assets/pictures/bulletA.png")) {
        std::cerr << "Failed to load bullet sprite!\n";
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return EXIT_FAILURE;
    }

    // setup
    // Team 1 (red)
    Character player1_1(Vector2(100.0f, WORLD_H / 2.0f - 50.0f), red_texture, 100.0f, 100.0f);
    Character player1_2(Vector2(100.0f, WORLD_H / 2.0f + 50.0f), red_texture, 100.0f, 100.0f);
    AnimatedSprite idle(renderer, "assets/pictures/PlayerIdle.png", 24, 16, 5, 100);
    AnimatedSprite run(renderer,  "assets/pictures/PlayerRunning.png", 24, 16, 5, 100);
    AnimatedSprite shoot(renderer,"assets/pictures/PlayerShooting.png", 24, 16, 5, 100);
    AnimatedSprite idle1(renderer, "assets/pictures/tocvangdung.png", 24, 16, 5, 100);
    AnimatedSprite run1(renderer,  "assets/pictures/tocvangchay.png", 24, 16, 5, 100);
    AnimatedSprite shoot1(renderer,"assets/pictures/tocvangban.png", 24, 16, 5, 100);
    player1_1.set_animations(&idle, &run, &shoot);
    player1_2.set_animations(&idle1, &run1, &shoot1);
    
    // Team 2 (blue)
    Character player2_1(Vector2(WORLD_W - 100.0f, WORLD_H / 2.0f - 50.0f), blue_texture, 100.0f, 100.0f);
    Character player2_2(Vector2(WORLD_W - 100.0f, WORLD_H / 2.0f + 50.0f), blue_texture, 100.0f, 100.0f);
    player2_1.set_animations(&idle, &run, &shoot);
    player2_2.set_animations(&idle1, &run1, &shoot1);
    std::vector<Character*> characters;
    characters.push_back(&player1_1);
    characters.push_back(&player1_2);
    characters.push_back(&player2_1);
    characters.push_back(&player2_2);

    InputHandler input_handler(InputSet::INPUT_1, &player1_1, &player1_2);
    InputHandler input_handler2(InputSet::INPUT_2, &player2_1, &player2_2);

    BlackHole blackhole(Vector2(WORLD_W / 2.0f, WORLD_H / 2.0f), 
                    nullptr, // No sprite for now, it will be debug-drawn
                    120.0f, 60.0f, // outer radius, inner radius
                    5.0f, 15.0f);  // outer dps, inner dps

    std::vector<Bullet*> bullet_list;

    std::vector<IUpdatable*> updatable_list;
    for (Character* ch : characters) {
        updatable_list.push_back(ch);
    }
    updatable_list.push_back(&input_handler);
    updatable_list.push_back(&input_handler2);
    updatable_list.push_back(&blackhole);
    //

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


        // check for collision
        for (auto& bullet : bullet_list) {
            blackhole.collide(*bullet);
            for (auto& character : characters) {
                character->collide(*bullet);
            }
        }

        for (auto& character : characters) {
            blackhole.collide(*character);
        }

        // --- Rendering ---
        SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF); // Set draw color to white
        SDL_RenderClear(renderer);                            // Clear the screen

        // Render character sprites
        for (auto& character : characters) {
            Vector2 pos = character->get_position();
            const SDL_Texture* tex = character->get_sprite();
            if (tex) {
                SDL_Rect dstRect = { (int)pos.x - 8, (int)pos.y - 8, 16, 16 };
                SDL_RenderCopy(renderer, const_cast<SDL_Texture*>(tex), NULL, &dstRect);
            }
        }

        // render bullets
        for (Bullet* bullet : bullet_list) {
            IRenderable* obj = dynamic_cast<IRenderable*>(bullet);
            obj->render(renderer);
        }
        // render hitbox blackhole
        for (HitBox* hb : blackhole.get_hitboxes()) {
            hb->debug_draw(renderer, {0, 0, 255, 255}); // Blue to see it clearly
        }



        // Render activation circles on top of everything
        for (auto& character : characters) {
            character->render(renderer);
        }


        SDL_RenderPresent(renderer);                          // Update the screen
    }

    resource_manager.unload_all();

    // Quit SDL
    SDL_DestroyTexture(red_texture);
    SDL_DestroyTexture(blue_texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return EXIT_SUCCESS;
}
