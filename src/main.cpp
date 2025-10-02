#include <SDL2/SDL.h>
#include <SDL2/SDL_error.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_video.h>
#include <SDL2/SDL_image.h>
#include "ResourceManager.h"
#include <SDL2/SDL_ttf.h>
#include <cstdlib>
#include <iostream>
#include "Constant.h"
#include <cmath>
#include <vector>
#include <algorithm>

// Game components used by the menu/game runner
#include "components/inc/AnimatedSprite.h"
#include "components/inc/Character.h"
#include "components/inc/InputHandler.h"
#include "components/inc/Wall.h"
#include "components/inc/Explosion.h"
#include "components/inc/Bullet.h"
#include "components/inc/BlackHole.h"
#include <random>

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

    // --- Menu setup ---
    bool running = true;
    SDL_Event event;

    // Try to load optional background and title textures
    resourceManager.load_texture("menu_bg", "assets/pictures/menu_bg.png");
    resourceManager.load_texture("title", "assets/pictures/title.png");
    SDL_Texture* bg = resourceManager.get_texture("menu_bg");
    SDL_Texture* title_tex = resourceManager.get_texture("title");

    const std::vector<std::string> options = { "PVP", "PVE", "Exit" };
    int selected = 0;

    // Initialize TTF for text rendering
    if (TTF_Init() == -1) {
        std::cerr << "TTF_Init failed: " << TTF_GetError() << "\n";
        // Not fatal: continue without text
    }

    // Try to load a font from a list of candidate locations
    TTF_Font* font = nullptr;
    std::vector<std::string> fontCandidates = {
        "assets/fonts/Roboto-Regular.ttf",
        "assets/fonts/arial.ttf",
        "assets/pictures/Roboto-Regular.ttf",
        "assets/pictures/arial.ttf",
        "C:/Windows/Fonts/arial.ttf",
        "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
        "/usr/share/fonts/truetype/liberation/LiberationSans-Regular.ttf",
        "/usr/local/share/fonts/DejaVuSans.ttf"
    };
    for (const auto& p : fontCandidates) {
        font = TTF_OpenFont(p.c_str(), 24);
        if (font) {
            std::cerr << "Loaded font: " << p << "\n";
            break;
        } else {
            std::cerr << "TTF_OpenFont failed for: " << p << " -> " << TTF_GetError() << "\n";
        }
    }
    if (!font) {
        std::cerr << "Warning: Unable to load any font for menu text.\n";
        std::cerr << "Place a .ttf file in 'assets/fonts/' (e.g. Roboto-Regular.ttf) or ensure a system font (arial, DejaVuSans) is available.\n";
    }

    auto run_placeholder_game = [&](const std::string& mode) {
        bool in_game = true;
        Uint32 last = SDL_GetTicks();
        while (in_game) {
            SDL_Event e;
            while (SDL_PollEvent(&e)) {
                if (e.type == SDL_QUIT) {
                    in_game = false; running = false; break;
                }
                if (e.type == SDL_KEYDOWN && e.key.keysym.scancode == SDL_SCANCODE_ESCAPE) {
                    in_game = false; // return to menu
                }
            }
            Uint32 now = SDL_GetTicks();
            float dt = (now - last) / 1000.0f; last = now;

            // Simple placeholder rendering
            SDL_SetRenderDrawColor(renderer, 0x10, 0x10, 0x10, 0xFF);
            SDL_RenderClear(renderer);
            // Draw mode name box
            SDL_Rect box = { WINDOW_W/2 - 150, WINDOW_H/2 - 25, 300, 50 };
            SDL_SetRenderDrawColor(renderer, 0x40, 0x40, 0x40, 0xFF);
            SDL_RenderFillRect(renderer, &box);
            // You can render more game systems here; press ESC to go back

            // Draw a boundary that matches the actual window edges: switch to window logical size,
            // draw a rect in window pixels, present, and restore logical size for the next frame.
            SDL_RenderSetLogicalSize(renderer, WINDOW_W, WINDOW_H);
            SDL_SetRenderDrawColor(renderer, 0xFF, 0x00, 0x00, 0xFF);
            SDL_Rect windowRect = { 0, 0, WINDOW_W, WINDOW_H };
            SDL_RenderDrawRect(renderer, &windowRect);
            SDL_RenderPresent(renderer);
            // restore world logical size for next frame
            SDL_RenderSetLogicalSize(renderer, WORLD_W, WORLD_H);
            SDL_Delay(16);
        }
    };

    // Forward-declare a real PVP runner that spawns 4 players and basic world bounds.
    auto run_pvp_game = [&](void) {
        // Initialize TTF if not already
        // create small game world similar to tests/test_char.cpp
        ResourceManager rm(renderer);

        // Load bullet texture (already loaded earlier but ensure available in RM)
        rm.load_texture("bullet", "assets/pictures/bulletA.png");

        // Create simple team textures (solid colored textures)
        SDL_Surface* red_surface = SDL_CreateRGBSurface(0, 16, 16, 32,
            0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);
        SDL_FillRect(red_surface, NULL, SDL_MapRGBA(red_surface->format, 255, 0, 0, 255));
        SDL_Texture* red_texture = SDL_CreateTextureFromSurface(renderer, red_surface);
        SDL_FreeSurface(red_surface);

        SDL_Surface* blue_surface = SDL_CreateRGBSurface(0, 16, 16, 32,
            0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);
        SDL_FillRect(blue_surface, NULL, SDL_MapRGBA(blue_surface->format, 0, 0, 255, 255));
        SDL_Texture* blue_texture = SDL_CreateTextureFromSurface(renderer, blue_surface);
        SDL_FreeSurface(blue_surface);

        // Animated sprites placeholders (nullptr accepted by Character constructor for sprite param)
        AnimatedSprite idle(renderer, "assets/pictures/PlayerIdle.png", 24, 16, 5, 100);
        AnimatedSprite run(renderer,  "assets/pictures/PlayerRunning.png", 24, 16, 5, 100);
        AnimatedSprite shoot(renderer,"assets/pictures/PlayerShooting.png", 24, 16, 5, 100);
    // Black hole animation (match tests/test_char.cpp)
    AnimatedSprite blackhole_anim(renderer, "assets/pictures/output.png", 200, 200, 12, 100, 3);

        // Create four characters (two per team)
        Character p1(Vector2(100.0f, WORLD_H / 2.0f - 50.0f), red_texture, 200.0f, 100.0f);
        Character p2(Vector2(100.0f, WORLD_H / 2.0f + 50.0f), red_texture, 200.0f, 100.0f);
        Character p3(Vector2(WORLD_W - 100.0f, WORLD_H / 2.0f - 50.0f), blue_texture, 200.0f, 100.0f);
        Character p4(Vector2(WORLD_W - 100.0f, WORLD_H / 2.0f + 50.0f), blue_texture, 200.0f, 100.0f);

        p1.set_animations(&idle, &run, &shoot);
        p2.set_animations(&idle, &run, &shoot);
        p3.set_animations(&idle, &run, &shoot);
        p4.set_animations(&idle, &run, &shoot);

        std::vector<Character*> characters = { &p1, &p2, &p3, &p4 };

        // Input handlers (assign two characters per input set)
        InputHandler ih1(InputSet::INPUT_1, &p1, &p2);
        InputHandler ih2(InputSet::INPUT_2, &p3, &p4);

        // Walls: create four walls forming bounds. Use simple colored textures
        SDL_Surface* wall_surf = SDL_CreateRGBSurface(0, 100, 100, 32, 0x00FF0000,0x0000FF00,0x000000FF,0xFF000000);
        SDL_FillRect(wall_surf, NULL, SDL_MapRGBA(wall_surf->format, 80, 80, 80, 255));
        SDL_Texture* wall_tex = SDL_CreateTextureFromSurface(renderer, wall_surf);
        SDL_FreeSurface(wall_surf);

        // Create four Wall objects
        Wall topWall(Vector2(WORLD_W/2.0f, 0.0f + 16.0f), wall_tex);
        Wall bottomWall(Vector2(WORLD_W/2.0f, WORLD_H - 16.0f), wall_tex);
        Wall leftWall(Vector2(0.0f + 16.0f, WORLD_H/2.0f), wall_tex);
        Wall rightWall(Vector2(WORLD_W - 16.0f, WORLD_H/2.0f), wall_tex);

        std::vector<IUpdatable*> updatables;
        for (auto* c : characters) updatables.push_back(c);
        updatables.push_back(&ih1);
        updatables.push_back(&ih2);
        updatables.push_back(&topWall);
        updatables.push_back(&bottomWall);
        updatables.push_back(&leftWall);
        updatables.push_back(&rightWall);

        std::vector<Bullet*> bullets;
        std::vector<Explosion*> explosions;

        // blackholes: pair<BlackHole*, spawn_time_ms>
        std::vector<std::pair<BlackHole*, Uint32>> blackholes;

        // RNG for blackhole spawn
        std::random_device rd;
        std::mt19937 rng(rd());
        std::uniform_real_distribution<float> distX(100.0f, WORLD_W - 100.0f);
        std::uniform_real_distribution<float> distY(100.0f, WORLD_H - 100.0f);
        const Uint32 blackhole_precaution_ms = 5000; // 5 seconds before first spawn
        const Uint32 blackhole_interval_ms = 30000; // spawn every 30 seconds
        const Uint32 blackhole_life_ms = 15000; // blackhole exists for 15 seconds
        Uint32 start_time = SDL_GetTicks();
        Uint32 last_bh_spawn = 0; // 0 indicates not spawned yet

        // Mark first players as activated (for rendering active circle)
        p1.set_activate(true);
        p3.set_activate(true);

    // set renderer logical size so world coordinates map to window
    SDL_RenderSetLogicalSize(renderer, WORLD_W, WORLD_H);

        // game loop
        bool in_game = true;
        Uint32 last = SDL_GetTicks();
        while (in_game) {
            SDL_Event e;
            while (SDL_PollEvent(&e)) {
                if (e.type == SDL_QUIT) { in_game = false; running = false; break; }
                if (e.type == SDL_KEYDOWN && e.key.keysym.scancode == SDL_SCANCODE_ESCAPE) { in_game = false; break; }
                if (e.type == SDL_KEYDOWN && e.key.keysym.scancode == SDL_SCANCODE_F11) {
                    // toggle fullscreen
                    Uint32 flags = SDL_GetWindowFlags(window);
                    if (flags & SDL_WINDOW_FULLSCREEN) SDL_SetWindowFullscreen(window, 0);
                    else SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);
                }
                // Pass events to input handlers which will add bullets to bullets vector
                ih1.handle_event(e, bullets, rm);
                ih2.handle_event(e, bullets, rm);
                if (e.type == SDL_KEYDOWN && e.key.keysym.scancode == SDL_SCANCODE_E) {
                    // spawn an explosion at center for testing
                    Vector2 pos(WORLD_W/2.0f - 50.0f, WORLD_H/2.0f - 50.0f);
                    Explosion* ex = new Explosion(renderer, "assets/pictures/rielno.png", pos, 50, 50, 9, 40, 3);
                    explosions.push_back(ex);
                }
            }

            Uint32 now = SDL_GetTicks();
            float dt = (now - last) / 1000.0f; last = now;

            for (auto* u : updatables) u->update(dt);
            for (auto& bhp : blackholes) if (bhp.first) bhp.first->update(dt);
            for (auto* b : bullets) b->update(dt);
            for (auto* ex : explosions) ex->update(dt);

            // remove finished explosions
            explosions.erase(std::remove_if(explosions.begin(), explosions.end(), [](Explosion* e){ if (e->is_finished()) { delete e; return true; } return false; }), explosions.end());

            // Blackhole spawn & lifetime logic
            Uint32 now_bh = SDL_GetTicks();
            // Start spawning only after precaution
            if (now_bh - start_time >= blackhole_precaution_ms) {
                if (last_bh_spawn == 0 || now_bh - last_bh_spawn >= blackhole_interval_ms) {
                    bool ok = false; int attempts = 0; Vector2 p;
                    while (!ok && attempts < 20) {
                        p.x = distX(rng);
                        p.y = distY(rng);
                        ok = true;
                        for (auto* c : characters) {
                            float dx = c->get_position().x - p.x;
                            float dy = c->get_position().y - p.y;
                            if (dx*dx + dy*dy < 200.0f * 200.0f) { ok = false; break; }
                        }
                        attempts++;
                    }
                    if (ok) {
                        BlackHole* nb = new BlackHole(p, nullptr, 65.0f, 30.0f, 5.0f, 15.0f);
                        nb->set_animation(&blackhole_anim);
                        blackholes.emplace_back(nb, now_bh);
                    }
                    last_bh_spawn = now_bh;
                }
            }

            // Remove expired blackholes
            for (auto& bhp : blackholes) {
                if (bhp.first && now_bh - bhp.second >= blackhole_life_ms) {
                    delete bhp.first;
                    bhp.first = nullptr;
                }
            }
            blackholes.erase(std::remove_if(blackholes.begin(), blackholes.end(), [](const std::pair<BlackHole*,Uint32>& p){ return p.first == nullptr; }), blackholes.end());

            
            // collisions: bullets vs characters/walls
            for (auto* b : bullets) {
                for (auto* c : characters) c->collide(*b);
                topWall.collide(*b); bottomWall.collide(*b); leftWall.collide(*b); rightWall.collide(*b);
                for (auto& bhp : blackholes) if (bhp.first) bhp.first->collide(*b);
            }
            for (auto& bhp : blackholes) if (bhp.first) {
                for (auto* c : characters) bhp.first->collide(*c);
            }

            // render
            SDL_SetRenderDrawColor(renderer, 0,0,0,255);
            SDL_RenderClear(renderer);

            // draw world boundary (visible)
            SDL_SetRenderDrawColor(renderer, 0xFF, 0x00, 0x00, 0xFF);
            SDL_Rect worldRect = { 0, 0, WORLD_W, WORLD_H };
            SDL_RenderDrawRect(renderer, &worldRect);

            // render walls (they also have hitboxes)
            topWall.render(renderer);
            bottomWall.render(renderer);
            leftWall.render(renderer);
            rightWall.render(renderer);

            // draw characters
            for (auto* c : characters) {
                c->render(renderer);
            }

            for (auto* b : bullets) {
                b->render(renderer);
            }

            for (auto* ex : explosions) { ex->render(renderer); }
            for (auto& bhp : blackholes) if (bhp.first) bhp.first->render(renderer);

            SDL_RenderPresent(renderer);
            SDL_Delay(16);
        }

    // restore renderer logical size back to window for menu
    SDL_RenderSetLogicalSize(renderer, WINDOW_W, WINDOW_H);

    // cleanup textures we created
        SDL_DestroyTexture(red_texture);
        SDL_DestroyTexture(blue_texture);
        SDL_DestroyTexture(wall_tex);
    // cleanup blackholes
    for (auto& bhp : blackholes) if (bhp.first) delete bhp.first;
    blackholes.clear();
        rm.unload_all();
    };

    // Show system cursor for menu interactivity
    SDL_ShowCursor(SDL_ENABLE);

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            } else if (event.type == SDL_KEYDOWN) {
                switch (event.key.keysym.scancode) {
                    case SDL_SCANCODE_F11:
                        {
                            Uint32 flags = SDL_GetWindowFlags(window);
                            if (flags & SDL_WINDOW_FULLSCREEN) SDL_SetWindowFullscreen(window, 0);
                            else SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);
                        }
                        break;
                    case SDL_SCANCODE_W:
                    case SDL_SCANCODE_UP:
                        selected = (selected - 1 + (int)options.size()) % (int)options.size();
                        break;
                    case SDL_SCANCODE_S:
                    case SDL_SCANCODE_DOWN:
                        selected = (selected + 1) % (int)options.size();
                        break;
                    case SDL_SCANCODE_RETURN:
                    case SDL_SCANCODE_KP_ENTER:
                        if (options[selected] == "Exit") {
                            running = false;
                        } else if (options[selected] == "PVP") {
                            run_pvp_game();
                        } else {
                            run_placeholder_game(options[selected]);
                        }
                        break;
                    default:
                        break;
                }
            } else if (event.type == SDL_MOUSEMOTION || event.type == SDL_MOUSEBUTTONDOWN) {
                int mx = 0, my = 0;
                if (event.type == SDL_MOUSEMOTION) {
                    mx = event.motion.x; my = event.motion.y;
                } else {
                    mx = event.button.x; my = event.button.y;
                }

                // compute option rectangles to hit-test
                int baseY = 220;
                bool clicked = (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT);
                for (size_t i = 0; i < options.size(); ++i) {
                    SDL_Rect opt = { WINDOW_W/2 - 100, baseY + (int)i * 70, 200, 50 };
                    if (mx >= opt.x && mx <= opt.x + opt.w && my >= opt.y && my <= opt.y + opt.h) {
                        selected = (int)i;
                        if (clicked) {
                            if (options[selected] == "Exit") {
                                running = false;
                            } else if (options[selected] == "PVP") {
                                run_pvp_game();
                            } else {
                                run_placeholder_game(options[selected]);
                            }
                        }
                        break;
                    }
                }
            }
        }

        // Render menu
        if (bg) {
            // draw background stretched
            SDL_Rect dst = {0,0, WINDOW_W, WINDOW_H};
            SDL_RenderCopy(renderer, bg, NULL, &dst);
        } else {
            SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
            SDL_RenderClear(renderer);
        }

        // Title
        if (title_tex) {
            int tw, th; SDL_QueryTexture(title_tex, NULL, NULL, &tw, &th);
            SDL_Rect tdst = { WINDOW_W/2 - tw/2, 80, tw, th };
            SDL_RenderCopy(renderer, title_tex, NULL, &tdst);
        } else {
            SDL_SetRenderDrawColor(renderer, 0x66, 0x66, 0x66, 0xFF);
            SDL_Rect tdst = { WINDOW_W/2 - 200, 60, 400, 80 };
            SDL_RenderFillRect(renderer, &tdst);
        }

        // Options (draw rectangles and text)
        int baseY = 220;
        for (size_t i = 0; i < options.size(); ++i) {
            SDL_Rect opt = { WINDOW_W/2 - 100, baseY + (int)i * 70, 200, 50 };
            if ((int)i == selected) {
                SDL_SetRenderDrawColor(renderer, 0xFF, 0xAA, 0x00, 0xFF);
            } else {
                SDL_SetRenderDrawColor(renderer, 0x44, 0x44, 0x44, 0xFF);
            }
            SDL_RenderFillRect(renderer, &opt);
            // Draw option text if font available
            if (font) {
                SDL_Color textColor = { 255, 255, 255, 255 };
                SDL_Surface* textSurf = TTF_RenderText_Blended(font, options[i].c_str(), textColor);
                if (textSurf) {
                    SDL_Texture* textTex = SDL_CreateTextureFromSurface(renderer, textSurf);
                    int tw = textSurf->w, th = textSurf->h;
                    SDL_FreeSurface(textSurf);
                    if (textTex) {
                        SDL_Rect tdst = { opt.x + (opt.w - tw)/2, opt.y + (opt.h - th)/2, tw, th };
                        SDL_RenderCopy(renderer, textTex, NULL, &tdst);
                        SDL_DestroyTexture(textTex);
                    }
                }
            }
        }

        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }

    // Quit SDL
    // ResourceManager will clean up textures automatically
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    if (font) TTF_CloseFont(font);
    TTF_Quit();
    SDL_Quit();
    return EXIT_SUCCESS;
}

