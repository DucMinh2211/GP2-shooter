#include <SDL2/SDL.h>
#include <SDL2/SDL_error.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_video.h>
#include <SDL2/SDL_ttf.h>
#include <cstdlib>
#include <iostream>
#include <vector>
#include "Constant.h"
#include "ResourceManager.h"
#include "math/Vector2.h"
#include "components/inc/Character.h"
#include "components/inc/IRenderable.h"
#include "components/inc/IUpdatable.h"
#include "components/inc/InputHandler.h"
#include "components/inc/Bullet.h"
#include "components/inc/BlackHole.h"
#include "components/inc/AnimatedSprite.h"
#include "components/inc/BuffItem.h"
#include "components/inc/Explosion.h"
#include "components/inc/Wall.h"
#include "components/inc/BloodSplash.h"
#include "components/inc/Smoke.h"
#include <string>
#include <sstream>




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

    // Initialize TTF
    if (TTF_Init() == -1) {
        std::cerr << "TTF_Init failed: " << TTF_GetError() << std::endl;
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return EXIT_FAILURE;
    }

    TTF_Font* font = TTF_OpenFont("assets/fonts/Slabo27px-Regular.ttf", 24);
    if (font == nullptr) {
        std::cerr << "Failed to load font: " << TTF_GetError() << std::endl;
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        TTF_Quit();
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

    // Create a green texture for buff items
    SDL_Surface* green_surface = SDL_CreateRGBSurface(0, 16, 16, 32, 0, 255, 0, 0);
    SDL_FillRect(green_surface, NULL, SDL_MapRGB(green_surface->format, 0, 255, 0));
    SDL_Texture* green_texture = SDL_CreateTextureFromSurface(renderer, green_surface);
    SDL_FreeSurface(green_surface);
    if (green_texture == nullptr) {
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
        SDL_Quit();
        return EXIT_FAILURE;
    }

    // Walls: create four walls forming bounds. Use simple colored textures
    SDL_Surface* wall_surf = SDL_CreateRGBSurface(0, 100, 100, 32, 0x00FF0000,0x0000FF00,0x000000FF,0xFF000000);
    SDL_FillRect(wall_surf, NULL, SDL_MapRGBA(wall_surf->format, 80, 80, 80, 255));
    SDL_Texture* wall_tex = SDL_CreateTextureFromSurface(renderer, wall_surf);
    SDL_FreeSurface(wall_surf);

    Wall far_wall(Vector2(WORLD_W / 2.0f, WORLD_H - 100), wall_tex);

    // setup
    // Team 1 (red)
    
    Character player1_1(Vector2(100.0f, WORLD_H / 2.0f - 50.0f), red_texture, 200.0f, 100.0f);
    Character player1_2(Vector2(100.0f, WORLD_H / 2.0f + 90.0f), red_texture, 200.0f, 100.0f);
    AnimatedSprite idle(renderer, "assets/pictures/PlayerIdle.png", 24, 16, 5, 100);
    AnimatedSprite run(renderer,  "assets/pictures/PlayerRunning.png", 24, 16, 5, 100);
    AnimatedSprite shoot(renderer,"assets/pictures/PlayerShooting.png", 24, 16, 5, 100);
    AnimatedSprite idle1(renderer, "assets/pictures/tocvangdung.png", 24, 16, 5, 100);
    AnimatedSprite run1(renderer,  "assets/pictures/tocvangchay.png", 24, 16, 5, 100);
    AnimatedSprite shoot1(renderer,"assets/pictures/tocvangban.png", 24, 16, 5, 100);
    AnimatedSprite blackhole_anim(renderer, "assets/pictures/output.png", 200, 200, 12, 100,3);
    player1_1.set_animations(&idle, &run, &shoot);
    player1_2.set_animations(&idle1, &run1, &shoot1);
    
    // Team 2 (blue)
    Character player2_1(Vector2(WORLD_W - 100.0f, WORLD_H / 2.0f - 50.0f), blue_texture, 100.0f, 100.0f);
    Character player2_2(Vector2(WORLD_W - 100.0f, WORLD_H / 2.0f + 90.0f), blue_texture, 100.0f, 100.0f);
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
                    65.0f, 30.0f, // outer radius, inner radius
                    5.0f, 15.0f);  // outer dps, inner dps
    blackhole.set_animation(&blackhole_anim);

    SDL_Texture* health_buff_texture = resource_manager.load_texture("health-buff", "assets/pictures/health-buff.png");
    BuffItem* health_buff = new BuffItem(Vector2(100, WORLD_H - 100.0f), health_buff_texture ? health_buff_texture : green_texture, CharBuffType::HEALTH);

    BuffItem* bounce_buff = new BuffItem(Vector2(160, WORLD_H - 100.0f), green_texture, BulletBuffType::BOUNCING);

    std::vector<Bullet*> bullet_list;
    // Explosions for testing (150x150 frames, 12 frames, 3 columns)
    std::vector<Explosion*> explosions;
    std::vector<BloodSplash*> bloods;
    std::vector<Smoke*> smokes;
    

    std::vector<IUpdatable*> updatable_list;
    for (Character* ch : characters) {
        updatable_list.push_back(ch);
    }
    updatable_list.push_back(&input_handler);
    updatable_list.push_back(&input_handler2);
    updatable_list.push_back(&blackhole);
    updatable_list.push_back(health_buff);
    updatable_list.push_back(bounce_buff);
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
                // Spawn explosion on E key press
                if (event.type == SDL_KEYDOWN && event.key.keysym.scancode == SDL_SCANCODE_E) {
                    Vector2 pos = blackhole.get_position() - Vector2(50, 50); 
                    Explosion* e = new Explosion(renderer, "assets/pictures/rielno.png", pos, 50, 50, 9, 40, 3);
                    explosions.push_back(e);
                }
                // Spawn blood splash on J, smoke on K for testing
                if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_j) {
                    Vector2 pos = player1_1.get_position();
                    BloodSplash* b = new BloodSplash(renderer, "assets/pictures/blood.png", pos, 16, 16, 8, 80, 3);
                    bloods.push_back(b);
                }
                if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_k) {
                    Vector2 pos = player2_1.get_position();
                    Smoke* s = new Smoke(renderer, "assets/pictures/khoi.png", pos, 24, 24, 8, 80, 3);
                    smokes.push_back(s);
                }
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

        // Update explosions and remove finished ones
        for (auto* e : explosions) e->update(delta_time);
    for (auto* b : bloods) b->update(delta_time);
    for (auto* s : smokes) s->update(delta_time);
        // remove finished
        explosions.erase(std::remove_if(explosions.begin(), explosions.end(), [](Explosion* e){
            if (e->is_finished()) { delete e; return true; }
            return false;
        }), explosions.end());
        // remove finished bloods
        bloods.erase(std::remove_if(bloods.begin(), bloods.end(), [](BloodSplash* b){ if (b->is_finished()) { delete b; return true; } return false; }), bloods.end());
        // remove finished smokes
        smokes.erase(std::remove_if(smokes.begin(), smokes.end(), [](Smoke* s){ if (s->is_finished()) { delete s; return true; } return false; }), smokes.end());


        // check for collision
        for (auto& bullet : bullet_list) {
            blackhole.collide(bullet);
            far_wall.collide(bullet);
            for (auto& character : characters) {
                character->collide(bullet);
            }
        }

        for (auto& character : characters) {
            blackhole.collide(character);
            if (health_buff) {
                health_buff->collide(character);
                character->collide(health_buff);
            }
            if (bounce_buff) {
                bounce_buff->collide(character);
                character->collide(bounce_buff);
            }
            // Wall collision
            character->collide(&far_wall);
        }

        // Handle consumed buff item
        if (health_buff && health_buff->is_consumed()) {
            // Remove from updatable_list
            for (size_t i = 0; i < updatable_list.size(); ++i) {
                if (updatable_list[i] == health_buff) {
                    updatable_list.erase(updatable_list.begin() + i);
                    break;
                }
            }
            delete health_buff;
            health_buff = nullptr;
        }

        // Handle destroyed bullets
        bullet_list.erase(std::remove_if(bullet_list.begin(), bullet_list.end(),
            [](Bullet* bullet) {
                if (bullet->is_destroyed()) {
                    delete bullet;
                    return true;
                }
                return false;
            }),
            bullet_list.end());

        // --- Rendering ---
        SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0x00); // Set draw color to black
        SDL_RenderClear(renderer);                            // Clear the screen

        // Render character sprites
        // render bullets
        for (Bullet* bullet : bullet_list) {
            IRenderable* obj = dynamic_cast<IRenderable*>(bullet);
            obj->render(renderer);
        }
        // render black hole animation/sprite
        blackhole.render(renderer);
        far_wall.render(renderer);
    // render explosions
    for (auto* e : explosions) 
    {
        e->render(renderer);
        for (auto* hb : e->get_hitboxes()) {
            hb->debug_draw(renderer, {255, 0, 0, 255}); // Red to see it clearly
        }
    }
        // render smoke effects (behind characters)
        for (auto* s : smokes) s->render(renderer);
    
        // render hitbox blackhole
        // for (HitBox* hb : blackhole.get_hitboxes()) {
        //     hb->debug_draw(renderer, {0, 0, 255, 255}); // Blue to see it clearly
        // }

        // for (HitBox* hb : player1_1.get_hitboxes()) {
        //     hb->debug_draw(renderer, {0, 0, 255, 255}); // Blue to see it clearly
        // }



        // Render activation circles and characters
        for (auto& character : characters) {
            character->render(renderer);
        }

        // Render blood effects on top of characters
        for (auto* b : bloods) b->render(renderer);

        if (health_buff) {
            health_buff->render(renderer);
        }
        if (bounce_buff) {
            bounce_buff->render(renderer);
        }

        // Render player health UI
        SDL_Color text_color = {255, 255, 255, 255}; // White color

        auto bulletBuffToString = [](BulletBuffType t){
            switch(t){
                case BulletBuffType::NONE: return "NONE";
                case BulletBuffType::BOUNCING: return "BOUNCING";
                case BulletBuffType::EXPLODING: return "EXPLODING";
                case BulletBuffType::PIERCING: return "PIERCING";
                default: return "UNKNOWN";
            }
        };

        struct PlayerInfo { const char* name; Character* ch; int x; int y; };
        std::vector<PlayerInfo> infos = {
            {"Player1_1", &player1_1, 10, 10},
            {"Player1_2", &player1_2, 10, 70},
            {"Player2_1", &player2_1, WORLD_W - 210, 10},
            {"Player2_2", &player2_2, WORLD_W - 210, 70}
        };

        for (auto& info : infos) {
            std::stringstream ss;
            ss << info.name << "\nHealth: " << (int)info.ch->get_health() << "\n" << bulletBuffToString(info.ch->get_gun_buff_type());
            SDL_Surface* surf = TTF_RenderText_Blended_Wrapped(font, ss.str().c_str(), text_color, 200);
            if (surf) {
                SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, surf);
                SDL_Rect dst = { info.x, info.y, surf->w, surf->h };
                SDL_FreeSurface(surf);
                if (tex) {
                    SDL_RenderCopy(renderer, tex, NULL, &dst);
                    SDL_DestroyTexture(tex);
                }
            }
        }

        SDL_RenderPresent(renderer);                          // Update the screen
    }

    for (Bullet* bullet : bullet_list) {
        delete bullet;
    }
    bullet_list.clear();

    if (health_buff) {
        delete health_buff;
    }
    resource_manager.unload_all();

    // Quit SDL
    SDL_DestroyTexture(red_texture);
    SDL_DestroyTexture(blue_texture);
    SDL_DestroyTexture(green_texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_CloseFont(font);
    TTF_Quit();
    SDL_Quit();
    return EXIT_SUCCESS;
}
