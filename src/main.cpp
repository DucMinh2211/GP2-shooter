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
#include "components/inc/BasicAI.h"
#include "components/inc/OBB.h"
#include "components/inc/InputHandler.h"
#include "components/inc/BloodSplash.h"
#include "components/inc/Smoke.h"
#include <unordered_map>
#include "components/inc/Wall.h"
#include "components/inc/Explosion.h"
#include "components/inc/Bullet.h"
#include "components/inc/BlackHole.h"
#include "components/inc/BuffItem.h"
#include <random>
#include <unordered_map>

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
    // Try PNG first, then JPG if PNG not present
    if (!resourceManager.load_texture("title", "assets/pictures/title.png")) {
        resourceManager.load_texture("title", "assets/pictures/title.jpg");
    }
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
            // placeholder: no timing used here

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
    // Load battlefield background for the world
    rm.load_texture("background", "assets/pictures/background.png");
    // Load buff textures so spawned buff items can use proper sprites
    rm.load_texture("health-buff", "assets/pictures/health-buff.png");
    rm.load_texture("bounce-buff", "assets/pictures/bounce-buff.png");
    rm.load_texture("explode-buff", "assets/pictures/explosion-buff.png");
    rm.load_texture("piercing-buff", "assets/pictures/piercing-buff.png");
    // Char speed buff texture
    rm.load_texture("speed-buff", "assets/pictures/speed-buff.png");

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
        AnimatedSprite idle1(renderer, "assets/pictures/tocvangdung.png", 24, 16, 5, 100);
        AnimatedSprite run1(renderer,  "assets/pictures/tocvangchay.png", 24, 16, 5, 100);
        AnimatedSprite shoot1(renderer,"assets/pictures/tocvangban.png", 24, 16, 5, 100);
    
    // Black hole animation (match tests/test_char.cpp)
    AnimatedSprite blackhole_anim(renderer, "assets/pictures/output.png", 200, 200, 12, 100, 3);

        // Create four characters (two per team)
        Character p1(Vector2(100.0f, WORLD_H / 2.0f - 50.0f), red_texture, 200.0f, 100.0f);
        Character p2(Vector2(100.0f, WORLD_H / 2.0f + 50.0f), red_texture, 200.0f, 100.0f);
        Character p3(Vector2(WORLD_W - 100.0f, WORLD_H / 2.0f - 50.0f), blue_texture, 200.0f, 100.0f);
        Character p4(Vector2(WORLD_W - 100.0f, WORLD_H / 2.0f + 50.0f), blue_texture, 200.0f, 100.0f);

    p1.set_animations(&idle, &run, &shoot);
    p2.set_animations(&idle1, &run1, &shoot1);
    p3.set_animations(&idle, &run, &shoot);
    p4.set_animations(&idle1, &run1, &shoot1);

    // Assign input sets / teams so bullets and collisions work correctly
    p1.set_input_set(0);
    p2.set_input_set(0);
    p3.set_input_set(1);
    p4.set_input_set(1);

    // Mark active players for rendering indicator (player team left, AI team right)
    p1.set_activate(true);
    p3.set_activate(true);

    std::vector<Character*> characters = { &p1, &p2, &p3, &p4 };

    // Fixed slot mapping for HUD stability: index 0=p1 (player1_1),1=p2 (player1_2),2=p3 (player2_1),3=p4 (player2_2)
    std::unordered_map<Character*, int> pvp_slot_index;
    pvp_slot_index[&p1] = 0;
    pvp_slot_index[&p2] = 1;
    pvp_slot_index[&p3] = 2;
    pvp_slot_index[&p4] = 3;

        // Input handlers (assign two characters per input set)
        InputHandler ih1(InputSet::INPUT_1, &p1, &p2);
        InputHandler ih2(InputSet::INPUT_2, &p3, &p4);

    // Walls: create four walls forming bounds. Make horizontal strips for top/bottom and vertical strips for left/right
    const int wall_thickness = 32;
    // Horizontal wall texture (width = WORLD_W, height = wall_thickness)
    SDL_Surface* wall_surf_h = SDL_CreateRGBSurface(0, WORLD_W, wall_thickness, 32, 0x00FF0000,0x0000FF00,0x000000FF,0xFF000000);
    SDL_FillRect(wall_surf_h, NULL, SDL_MapRGBA(wall_surf_h->format, 80, 80, 80, 255));
    SDL_Texture* wall_tex_h = SDL_CreateTextureFromSurface(renderer, wall_surf_h);
    SDL_FreeSurface(wall_surf_h);

    // Vertical wall texture (width = wall_thickness, height = WORLD_H)
    SDL_Surface* wall_surf_v = SDL_CreateRGBSurface(0, wall_thickness, WORLD_H, 32, 0x00FF0000,0x0000FF00,0x000000FF,0xFF000000);
    SDL_FillRect(wall_surf_v, NULL, SDL_MapRGBA(wall_surf_v->format, 80, 80, 80, 255));
    SDL_Texture* wall_tex_v = SDL_CreateTextureFromSurface(renderer, wall_surf_v);
    SDL_FreeSurface(wall_surf_v);

    // Create four Wall objects positioned to cover the edges
    Wall topWall(Vector2(WORLD_W/2.0f, wall_thickness / 2.0f), wall_tex_h);
    Wall bottomWall(Vector2(WORLD_W/2.0f, WORLD_H - wall_thickness / 2.0f), wall_tex_h);
    Wall leftWall(Vector2(wall_thickness / 2.0f, WORLD_H/2.0f), wall_tex_v);
    Wall rightWall(Vector2(WORLD_W - wall_thickness / 2.0f, WORLD_H/2.0f), wall_tex_v);

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
    std::vector<BloodSplash*> bloods;
    std::vector<Smoke*> smokes;
    // previous health tracking for death detection
    std::unordered_map<Character*, float> prev_health;
    for (auto c : characters) if (c) prev_health[c] = c->get_health();

        // blackholes: pair<BlackHole*, spawn_time_ms>
        std::vector<std::pair<BlackHole*, Uint32>> blackholes;

    // Random internal walls: generate 7 walls per stage (PVP)
    std::vector<Wall*> pvp_random_walls;
        // RNG for random walls and other game elements
        std::random_device rd;
        std::mt19937 rng(rd());
        std::uniform_real_distribution<float> wallX(150.0f, WORLD_W - 150.0f);
        std::uniform_real_distribution<float> wallY(150.0f, WORLD_H - 150.0f);
        std::uniform_int_distribution<int> wallW(64, 240);
        std::uniform_int_distribution<int> wallH(16, 96);
        for (int i = 0; i < 7; ++i) {
            int w = wallW(rng);
            int h = wallH(rng);
            bool placed = false;
            int attempts = 0;
            Vector2 chosenPos;
            // Try to find a position that doesn't intersect boundary walls or existing random walls
            while (!placed && attempts < 30) {
                Vector2 pos(wallX(rng), wallY(rng));
                // create a temporary OBB for the candidate wall
                OBB tmp_box(pos, Vector2(w / 2.0f, h / 2.0f), 0.0f);
                bool intersects = false;
                // check boundary walls
                for (auto* hb : topWall.get_hitboxes()) if (hb->is_collide(tmp_box)) { intersects = true; break; }
                if (intersects) { attempts++; continue; }
                for (auto* hb : bottomWall.get_hitboxes()) if (hb->is_collide(tmp_box)) { intersects = true; break; }
                if (intersects) { attempts++; continue; }
                for (auto* hb : leftWall.get_hitboxes()) if (hb->is_collide(tmp_box)) { intersects = true; break; }
                if (intersects) { attempts++; continue; }
                for (auto* hb : rightWall.get_hitboxes()) if (hb->is_collide(tmp_box)) { intersects = true; break; }
                if (intersects) { attempts++; continue; }

                // check existing random walls
                for (auto* existing : pvp_random_walls) if (existing) {
                    for (auto* hb : existing->get_hitboxes()) {
                        if (hb->is_collide(tmp_box)) { intersects = true; break; }
                    }
                    if (intersects) break;
                }
                // avoid spawning walls too close to player spawn positions
                if (!intersects) {
                    const float min_player_clearance = 150.0f; // pixels
                    for (auto* pc : characters) if (pc) {
                        float dx = pc->get_position().x - pos.x;
                        float dy = pc->get_position().y - pos.y;
                        if (dx*dx + dy*dy < min_player_clearance * min_player_clearance) { intersects = true; break; }
                    }
                }
                if (!intersects) {
                    placed = true;
                    chosenPos = pos;
                } else attempts++;
            }

            if (!placed) {
                // couldn't find a free spot after attempts; skip creating this wall
                continue;
            }

            // Create surface/texture only after a valid position is chosen
            SDL_Surface* surf = SDL_CreateRGBSurface(0, w, h, 32, 0x00FF0000,0x0000FF00,0x000000FF,0xFF000000);
            SDL_FillRect(surf, NULL, SDL_MapRGBA(surf->format, 100, 100, 100, 255));
            SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, surf);
            SDL_FreeSurface(surf);
            Wall* rw = new Wall(chosenPos, tex);
            pvp_random_walls.push_back(rw);
            updatables.push_back(rw);
        }

        // Buff items and timers
        std::vector<BuffItem*> buffs;
        const Uint32 buff_interval_ms = 10000; // spawn every 10s
        Uint32 last_buff_spawn = SDL_GetTicks();

    // Gun-change timer: rotate guns every 30s
    const Uint32 gun_change_ms = 30000; // 30 seconds
    Uint32 last_gun_change = SDL_GetTicks();

    // RNG for blackhole spawn (reuse rng declared above)
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
    bool debug_hitboxes = false;
        Uint32 last = SDL_GetTicks();
        int winning_team = -1; // 1 = red (team 1), 2 = blue (team 2)
    // simple on-screen notifications
    struct Notify { std::string text; Uint32 expiry; };
    std::vector<Notify> notifications;
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
                if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_b) debug_hitboxes = !debug_hitboxes;
                if (e.type == SDL_KEYDOWN && e.key.keysym.scancode == SDL_SCANCODE_E) {
                    // spawn an explosion at center for testing and a smoke
                    Vector2 pos(WORLD_W/2.0f - 50.0f, WORLD_H/2.0f - 50.0f);
                    Explosion* ex = new Explosion(renderer, "assets/pictures/rielno.png", pos, 50, 50, 9, 40, 3);
                    explosions.push_back(ex);
                    Smoke* s = new Smoke(renderer, "assets/pictures/khoi.png", pos, 64, 64, 8, 80, 4);
                    smokes.push_back(s);
                }
            }

            Uint32 now = SDL_GetTicks();
            float dt = (now - last) / 1000.0f; last = now;

            for (auto* u : updatables) u->update(dt);
            for (auto& bhp : blackholes) if (bhp.first) bhp.first->update(dt);
            for (auto* b : bullets) b->update(dt);
            for (auto* ex : explosions) ex->update(dt);
            // Explosion collisions: let characters and bullets react to explosions
            for (auto* ex : explosions) {
                for (auto* c : characters) if (c) c->collide(ex);
                for (auto* bl : bullets) if (bl) ex->collide(bl);
            }
            for (auto* b : bloods) b->update(dt);
            for (auto* s : smokes) s->update(dt);

            // (PVE blackhole logic belongs inside the PVE runner; removed stray block)

            // detect damage and deaths: spawn blood on hit, smoke on death, remove dead immediately
            std::vector<Character*> just_died;
            for (auto c : characters) {
                if (!c) continue;
                float old_h = prev_health[c];
                float new_h = c->get_health();
                // took damage (but still alive) -> spawn small blood splash
                if (new_h < old_h && new_h > 0.0f) {
                    Vector2 bpos = c->get_position();
                    // blood sprite: 24x24 frames, 8 frames, 80ms per frame, 3 columns
                    BloodSplash* bs = new BloodSplash(renderer, std::string("assets/pictures/blood.png"), bpos, 16, 16, 8, 80, 3);
                    bloods.push_back(bs);
                    
                }
                // just died -> spawn smoke and mark for removal
                if (old_h > 0.0f && new_h <= 0.0f) {
                    Vector2 spos = c->get_position();
                    Smoke* s = new Smoke(renderer, "assets/pictures/khoi.png", spos, 24, 24, 8, 80, 3);
                    smokes.push_back(s);
                    
                    just_died.push_back(c);
                    // notify input handlers to swap control or clear references
                    ih1.on_character_death(c);
                    ih2.on_character_death(c);
                }
                prev_health[c] = new_h;
            }
            // remove dead characters from active lists so they immediately disappear from HUD/world
            if (!just_died.empty()) {
                for (auto d : just_died) {
                    // remove from characters vector
                    characters.erase(std::remove(characters.begin(), characters.end(), d), characters.end());
                    // remove from updatables so they are no longer updated
                    updatables.erase(std::remove(updatables.begin(), updatables.end(), static_cast<IUpdatable*>(d)), updatables.end());
                    // remove prev_health entry
                    prev_health.erase(d);
                }
            }

            // remove finished explosions
            explosions.erase(std::remove_if(explosions.begin(), explosions.end(), [](Explosion* e){ if (e->is_finished()) { delete e; return true; } return false; }), explosions.end());
            // remove finished bloods
            bloods.erase(std::remove_if(bloods.begin(), bloods.end(), [](BloodSplash* b){ if (b->is_finished()) { delete b; return true; } return false; }), bloods.end());
            // remove finished smokes
            smokes.erase(std::remove_if(smokes.begin(), smokes.end(), [](Smoke* s){ if (s->is_finished()) { delete s; return true; } return false; }), smokes.end());

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

            
            // collisions: bullets vs blackholes/walls/characters (single-sample per bullet)
            for (auto* b : bullets) {
                if (!b) continue;
                // use current bullet position (Bullet::update() performs its own internal CCD if needed)
                for (auto& bhp : blackholes) if (bhp.first) bhp.first->collide(b);
                if (b->is_destroyed()) continue;

                topWall.collide(b); if (b->is_destroyed()) continue;
                bottomWall.collide(b); if (b->is_destroyed()) continue;
                leftWall.collide(b); if (b->is_destroyed()) continue;
                rightWall.collide(b); if (b->is_destroyed()) continue;
                for (auto* rw : pvp_random_walls) if (rw) { rw->collide(b); if (b->is_destroyed()) continue; }
                if (b->is_destroyed()) continue;

                for (auto* c : characters) if (c) { c->collide(b); if (b->is_destroyed()) break; }
            }

            // bullet vs bullet collisions: bullets from different teams destroy each other
            for (size_t i = 0; i < bullets.size(); ++i) {
                Bullet* a = bullets[i];
                if (!a || a->is_destroyed()) continue;
                for (size_t j = i + 1; j < bullets.size(); ++j) {
                    Bullet* b = bullets[j];
                    if (!b || b->is_destroyed()) continue;
                    if (a->get_team_id() == b->get_team_id()) continue;
                    bool collided = false;
                    for (auto* ah : a->get_hitboxes()) {
                        for (auto* bh : b->get_hitboxes()) {
                            if (ah->is_collide(*bh)) { collided = true; break; }
                        }
                        if (collided) break;
                    }
                    if (collided) {
                        a->set_destroyed(true);
                        b->set_destroyed(true);
                    }
                }
            }

            // remove destroyed bullets (explode if necessary) - follow tests/test_char.cpp
            bullets.erase(std::remove_if(bullets.begin(), bullets.end(), [&](Bullet* bullet){
                if (!bullet) return true;
                if (bullet->is_destroyed()) {
                    if (bullet->getBuff() == BulletBuffType::EXPLODING) {
                        bullet->explode(explosions, renderer);
                    }
                    delete bullet;
                    return true;
                }
                return false;
            }), bullets.end());
            for (auto& bhp : blackholes) if (bhp.first) {
                for (auto* c : characters) bhp.first->collide(c);
            }

            // Ensure characters collide with boundary walls and random internal walls so they cannot pass through
            for (auto* c : characters) {
                if (!c) continue;
                topWall.collide(c);
                bottomWall.collide(c);
                leftWall.collide(c);
                rightWall.collide(c);
                for (auto* rw : pvp_random_walls) if (rw) rw->collide(c);
            }

            // Buff spawn logic every 10s
            if (SDL_GetTicks() - last_buff_spawn >= buff_interval_ms) {
                // Choose a buff at random among all CharBuffType and BulletBuffType values.
                std::variant<CharBuffType, BulletBuffType> bt;
                // Only spawn a bullet buff if no character currently has a bullet buff active
                bool any_bullet_buff = false;
                for (auto* c : characters) if (c) {
                    if (c->get_gun_buff_type() != BulletBuffType::NONE) { any_bullet_buff = true; break; }
                }

                // Enumerate CharBuffType and BulletBuffType ranges
                const int num_char_buffs = (int)CharBuffType::NUM; // e.g., HEALTH, SPEED
                // For BulletBuffType, we don't have a NUM enumerator; infer by listing known values
                // Define an ordered array of available bullet buffs
                std::vector<BulletBuffType> bulletTypes = { BulletBuffType::BOUNCING, BulletBuffType::EXPLODING, BulletBuffType::PIERCING };

                // Decide whether to spawn a char buff or bullet buff with equal probability
                std::uniform_int_distribution<int> chooseType(0, 1);
                int kind = chooseType(rng);
                if (kind == 0) {
                    // Char buff: pick uniformly from CharBuffType values
                    std::uniform_int_distribution<int> chooseChar(0, num_char_buffs - 1);
                    int idx = chooseChar(rng);
                    bt = static_cast<CharBuffType>(idx);
                } else {
                    // Bullet buff: only if none currently active
                    if (any_bullet_buff) {
                        // fallback to a random char buff
                        std::uniform_int_distribution<int> chooseChar(0, num_char_buffs - 1);
                        int idx = chooseChar(rng);
                        bt = static_cast<CharBuffType>(idx);
                    } else {
                        std::uniform_int_distribution<int> chooseBullet(0, (int)bulletTypes.size() - 1);
                        int idx = chooseBullet(rng);
                        bt = bulletTypes[idx];
                    }
                }
                // pick a spawn position that does not intersect any wall hitbox
                Vector2 pos;
                bool placed = false;
                for (int attempt = 0; attempt < 30 && !placed; ++attempt) {
                    pos = Vector2(wallX(rng), wallY(rng));
                    // create a temporary OBB for the buff area (32x32)
                    OBB tmp_box(pos, Vector2(16.0f, 16.0f), 0.0f);
                    bool intersects = false;
                    // check boundary walls
                    for (auto* hb : topWall.get_hitboxes()) if (hb->is_collide(tmp_box)) { intersects = true; break; }
                    if (intersects) continue;
                    for (auto* hb : bottomWall.get_hitboxes()) if (hb->is_collide(tmp_box)) { intersects = true; break; }
                    if (intersects) continue;
                    for (auto* hb : leftWall.get_hitboxes()) if (hb->is_collide(tmp_box)) { intersects = true; break; }
                    if (intersects) continue;
                    for (auto* hb : rightWall.get_hitboxes()) if (hb->is_collide(tmp_box)) { intersects = true; break; }
                    if (intersects) continue;
                    // check random internal walls
                    for (auto* rw : pvp_random_walls) if (rw) {
                        for (auto* hb : rw->get_hitboxes()) {
                            if (hb->is_collide(tmp_box)) { intersects = true; break; }
                        }
                        if (intersects) break;
                    }
                    if (!intersects) placed = true;
                }
                if (placed) {
                    // select texture based on buff type (use ResourceManager textures when available)
                    SDL_Texture* chosen_tex = nullptr;
                    if (std::holds_alternative<CharBuffType>(bt)) {
                        CharBuffType cb = std::get<CharBuffType>(bt);
                        switch (cb) {
                            case CharBuffType::HEALTH: chosen_tex = rm.get_texture("health-buff"); break;
                            case CharBuffType::SPEED: chosen_tex = rm.get_texture("speed-buff"); break;
                            default: chosen_tex = rm.get_texture("health-buff"); break;
                        }
                    } else if (std::holds_alternative<BulletBuffType>(bt)) {
                        BulletBuffType bb = std::get<BulletBuffType>(bt);
                        switch (bb) {
                            case BulletBuffType::BOUNCING: chosen_tex = rm.get_texture("bounce-buff"); break;
                            case BulletBuffType::EXPLODING: chosen_tex = rm.get_texture("explode-buff"); break;
                            case BulletBuffType::PIERCING: chosen_tex = rm.get_texture("piercing-buff"); break;
                            default: chosen_tex = nullptr; break;
                        }
                    }

                    SDL_Texture* btex = chosen_tex;
                    // fallback: create a temporary orange texture if resource missing
                    if (!btex) {
                        SDL_Surface* bs = SDL_CreateRGBSurface(0, 32, 32, 32, 0x00FF0000,0x0000FF00,0x000000FF,0xFF000000);
                        SDL_FillRect(bs, NULL, SDL_MapRGBA(bs->format, 200, 100, 0, 255));
                        btex = SDL_CreateTextureFromSurface(renderer, bs);
                        SDL_FreeSurface(bs);
                    }

                    BuffItem* bi = new BuffItem(pos, btex, bt);
                    buffs.push_back(bi);
                    updatables.push_back(bi);
                }
                last_buff_spawn = SDL_GetTicks();
            }

            // Let characters check for buff collisions and remove consumed buffs (same logic as tests/test_char.cpp)
            for (auto* c : characters) {
                if (!c) continue;
                for (auto* bi : buffs) if (bi) c->collide(bi);
            }
            {
                auto it = buffs.begin();
                while (it != buffs.end()) {
                    BuffItem* bi = *it;
                    if (bi->is_consumed()) {
                        // If the consumed buff was a BulletBuff, clear bullet buffs from other characters
                        auto btype = bi->get_buff_type();
                        if (std::holds_alternative<BulletBuffType>(btype)) {
                            BulletBuffType taken = std::get<BulletBuffType>(btype);
                            // assign to the first character who overlaps (character::collide already set their buff),
                            // but ensure others have no bullet buff
                            for (auto* c : characters) if (c) {
                                if (c->get_gun_buff_type() != BulletBuffType::NONE && c->get_gun_buff_type() != taken) {
                                    c->clear_bullet_buff();
                                }
                            }
                        }
                        updatables.erase(std::remove(updatables.begin(), updatables.end(), static_cast<IUpdatable*>(bi)), updatables.end());
                        delete bi;
                        it = buffs.erase(it);
                    } else {
                        ++it;
                    }
                }
            }

            // Rotate guns every gun_change_ms (Pistol <-> AK)
            if (SDL_GetTicks() - last_gun_change >= gun_change_ms) {
                last_gun_change = SDL_GetTicks();
                // toggle and collect a message indicating new global gun
                for (auto* c : characters) {
                    if (!c) continue;
                    GunType cur = c->get_gun_type();
                    GunType next = GunType::PISTOL;
                    if (cur == GunType::PISTOL) next = GunType::AK;
                    else next = GunType::PISTOL;
                    c->set_gun_type(next);
                }
                // push a global notification for 2.5s
                std::string msg = "Guns switched!";
                notifications.push_back({ msg, SDL_GetTicks() + 2500 });
            }

            // Team win detection: check team membership via each character's input set (team id)
            bool red_alive = false, blue_alive = false;
            for (auto* ch : characters) {
                if (!ch) continue;
                if (ch->get_input_set() == 0) { if (ch->get_health() > 0.0f) red_alive = true; }
                else if (ch->get_input_set() == 1) { if (ch->get_health() > 0.0f) blue_alive = true; }
            }
            if (!red_alive || !blue_alive) {
                // determine winner: team 1 = input_set 0 (red), team 2 = input_set 1 (blue)
                if (red_alive && !blue_alive) winning_team = 1;
                else if (blue_alive && !red_alive) winning_team = 2;
                else winning_team = -1; // tie or all dead
                // exit main loop; we'll display the victory banner after the loop
                in_game = false;
            }

            // render
            SDL_SetRenderDrawColor(renderer, 0,0,0,255);
            SDL_RenderClear(renderer);

            // draw battlefield background (if available)
            SDL_Texture* bgtex = rm.get_texture("background");
            if (bgtex) {
                SDL_Rect dst = { 0, 0, WORLD_W, WORLD_H };
                SDL_RenderCopy(renderer, bgtex, NULL, &dst);
            }
            // draw world boundary (visible)
            SDL_SetRenderDrawColor(renderer, 0xFF, 0x00, 0x00, 0xFF);
            SDL_Rect worldRect = { 0, 0, WORLD_W, WORLD_H };
            SDL_RenderDrawRect(renderer, &worldRect);

            // render walls (they also have hitboxes)
            topWall.render(renderer);
            bottomWall.render(renderer);
            leftWall.render(renderer);
            rightWall.render(renderer);
            // render random internal walls
            for (auto* rw : pvp_random_walls) if (rw) rw->render(renderer);

            // draw characters
            for (auto* c : characters) {
                c->render(renderer);
            }

            for (auto* b : bullets) {
                b->render(renderer);
            }

            // debug: draw hitboxes
            if (debug_hitboxes) {
                // bullet hitboxes
                for (auto* bullet : bullets) {
                    for (auto* hb : bullet->get_hitboxes()) hb->debug_draw(renderer, {255, 0, 0, 255});
                }
                // wall hitboxes
                for (auto* rw : pvp_random_walls) if (rw) for (auto* hb : rw->get_hitboxes()) hb->debug_draw(renderer, {0,255,0,255});
                for (auto* hb : topWall.get_hitboxes()) hb->debug_draw(renderer, {0,255,0,255});
                for (auto* hb : bottomWall.get_hitboxes()) hb->debug_draw(renderer, {0,255,0,255});
                for (auto* hb : leftWall.get_hitboxes()) hb->debug_draw(renderer, {0,255,0,255});
                for (auto* hb : rightWall.get_hitboxes()) hb->debug_draw(renderer, {0,255,0,255});
            }

            for (auto* ex : explosions) { ex->render(renderer); }
            for (auto* b : bloods) b->render(renderer);
            for (auto* s : smokes) s->render(renderer);
            for (auto* bi : buffs) if (bi) bi->render(renderer);
            for (auto& bhp : blackholes) if (bhp.first) bhp.first->render(renderer);

            // UI overlay: split HUD into top-left and top-right panels
            if (font) {
                SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
                // render notifications (centered on screen)
                // compute how many valid notifications we have so we can vertically center the stack
                int validCount = 0;
                for (auto &n : notifications) if ((int)n.text.size() != 0 && SDL_GetTicks() <= n.expiry) ++validCount;
                int lineH = 20;
                int notifY = WORLD_H/2 - (validCount * lineH) / 2;
                for (auto it = notifications.begin(); it != notifications.end();) {
                    if ((int)it->text.size() == 0 || SDL_GetTicks() > it->expiry) { it = notifications.erase(it); continue; }
                    SDL_Color textColor = { 255, 220, 120, 255 };
                    SDL_Surface* t = TTF_RenderText_Blended(font, it->text.c_str(), textColor);
                    if (t) {
                        SDL_Texture* tt = SDL_CreateTextureFromSurface(renderer, t);
                        int tw = t->w, th = t->h;
                        SDL_Rect td = { WORLD_W/2 - tw/2, notifY, tw, th };
                        SDL_FreeSurface(t);
                        if (tt) { SDL_RenderCopy(renderer, tt, NULL, &td); SDL_DestroyTexture(tt); }
                    }
                    notifY += lineH;
                    ++it;
                }
                // Fixed two slots per team HUD to avoid shifting when players die
                const int panelW = 270; // smaller panel width
                const int entryH = 56;  // reduced entry height
                const int panelY = 8;
                const int left_count = 2;
                const int right_count = 2;
                // left panel background (team 1)
                int panelHLeft = 16 + left_count * entryH;
                int panelLeftX = 8;
                SDL_Rect panelLeftBg = { panelLeftX - 6, panelY - 6, panelW, panelHLeft };
                SDL_SetRenderDrawColor(renderer, 0, 0, 0, 160);
                SDL_RenderFillRect(renderer, &panelLeftBg);
                // right panel background (team 2)
                int panelHRight = 16 + right_count * entryH;
                int panelRightX = WORLD_W - panelW - 8;
                SDL_Rect panelRightBg = { panelRightX - 6, panelY - 6, panelW, panelHRight };
                SDL_SetRenderDrawColor(renderer, 0, 0, 0, 160);
                SDL_RenderFillRect(renderer, &panelRightBg);

                // Build per-team fixed slot lists (2 slots each) using the initial spawn mapping
                Character* team0_slots[2] = { nullptr, nullptr };
                Character* team1_slots[2] = { nullptr, nullptr };
                for (auto* ch : characters) {
                    if (!ch) continue;
                    auto it = pvp_slot_index.find(ch);
                    if (it != pvp_slot_index.end()) {
                        int idx = it->second;
                        if (idx == 0) team0_slots[0] = ch;
                        else if (idx == 1) team0_slots[1] = ch;
                        else if (idx == 2) team1_slots[0] = ch;
                        else if (idx == 3) team1_slots[1] = ch;
                    } else {
                        // Fallback: fill empty slots by team
                        if (ch->get_input_set() == 0) {
                            if (!team0_slots[0]) team0_slots[0] = ch;
                            else if (!team0_slots[1]) team0_slots[1] = ch;
                        } else {
                            if (!team1_slots[0]) team1_slots[0] = ch;
                            else if (!team1_slots[1]) team1_slots[1] = ch;
                        }
                    }
                }

                // Debug: print slot mapping and health when debug_hitboxes is enabled
                if (debug_hitboxes) {
                    for (int si = 0; si < 2; ++si) {
                        Character* c0 = team0_slots[si];
                        if (c0) SDL_Log("PVP HUD slot L%d -> ptr=%p health=%.1f input_set=%d", si, (void*)c0, c0->get_health(), c0->get_input_set());
                        else SDL_Log("PVP HUD slot L%d -> EMPTY", si);
                    }
                    for (int si = 0; si < 2; ++si) {
                        Character* c1 = team1_slots[si];
                        if (c1) SDL_Log("PVP HUD slot R%d -> ptr=%p health=%.1f input_set=%d", si, (void*)c1, c1->get_health(), c1->get_input_set());
                        else SDL_Log("PVP HUD slot R%d -> EMPTY", si);
                    }
                }

                // helper: ellipsize to pixel width
                auto ellipsize = [&](const std::string &full, int maxW) {
                    std::string s = full;
                    int w = 0, h = 0;
                    if (TTF_SizeText(font, s.c_str(), &w, &h) == 0 && w <= maxW) return s;
                    while (!s.empty()) {
                        s = s.substr(0, s.size() - 1);
                        std::string t = s + "...";
                        if (TTF_SizeText(font, t.c_str(), &w, &h) == 0 && w <= maxW) return t;
                    }
                    return std::string("...");
                };

                // left column entries (team 0) - fixed 2 slots
                for (int idx = 0; idx < 2; ++idx) {
                    Character* ch = team0_slots[idx];
                    int x = panelLeftX;
                    int y = panelY + idx * entryH;

                    SDL_Rect entryBg = { x + 8, y + 8, panelW - 16, entryH - 16 };
                    SDL_SetRenderDrawColor(renderer, 24, 24, 24, 200);
                    SDL_RenderFillRect(renderer, &entryBg);

                    SDL_Rect sw = { x + 12, y + 12, 18, 18 };
                    SDL_SetRenderDrawColor(renderer, 200, 60, 60, 255);
                    SDL_RenderFillRect(renderer, &sw);

                    std::string basename = (idx==0?"player1_1":"player1_2");
                    std::string fullName = basename + (ch ? (ch->get_gun_type() == GunType::AK ? " AK" : " PIS") : " (dead)");
                    int maxTextW = panelW - 28 - 48;
                    std::string nameToRender = ellipsize(fullName, maxTextW);
                    SDL_Color textColor = { 230, 230, 230, 255 };
                    int textH = 0;
                    if (ch) {
                        SDL_Surface* textSurf = TTF_RenderText_Blended(font, nameToRender.c_str(), textColor);
                        if (textSurf) {
                            textH = textSurf->h;
                            SDL_Texture* textTex = SDL_CreateTextureFromSurface(renderer, textSurf);
                            SDL_Rect dst = { x + 28, y + 10, textSurf->w, textH };
                            SDL_FreeSurface(textSurf);
                            if (textTex) { SDL_RenderCopy(renderer, textTex, NULL, &dst); SDL_DestroyTexture(textTex); }
                        }
                    } else {
                        // render dimmed placeholder name
                        SDL_Surface* textSurf = TTF_RenderText_Blended(font, nameToRender.c_str(), SDL_Color{160,160,160,255});
                        if (textSurf) {
                            textH = textSurf->h;
                            SDL_Texture* textTex = SDL_CreateTextureFromSurface(renderer, textSurf);
                            SDL_Rect dst = { x + 28, y + 10, textSurf->w, textH };
                            SDL_FreeSurface(textSurf);
                            if (textTex) { SDL_RenderCopy(renderer, textTex, NULL, &dst); SDL_DestroyTexture(textTex); }
                        }
                    }

                    float hp = ch ? std::max(0.0f, ch->get_health()) : 0.0f;
                    float hpfrac = std::min(1.0f, hp / 100.0f);
                    int hbW = 150;
                    int hbH = 12;
                    int hbY = y + 12 + textH + 8;
                    SDL_Rect hbBg = { x + 36, hbY, hbW, hbH };
                    SDL_SetRenderDrawColor(renderer, 60, 60, 60, 200); SDL_RenderFillRect(renderer, &hbBg);
                    SDL_Rect hbFg = { x + 36, hbY, (int)(hbW * hpfrac), hbH };
                    SDL_SetRenderDrawColor(renderer, (Uint8)(200 * (1.0f - hpfrac)), (Uint8)(200 * hpfrac), 50, 255); SDL_RenderFillRect(renderer, &hbFg);

                    int ix = x + panelW - 28;
                    int iconY = y + 10;
                    if (ch) {
                        auto cbs = ch->get_active_char_buffs();
                        for (auto cb : cbs) {
                            SDL_Texture* cbtex = nullptr;
                            switch (cb) {
                                case CharBuffType::HEALTH: cbtex = rm.get_texture("health-buff"); break;
                                case CharBuffType::SPEED: cbtex = rm.get_texture("speed-buff"); break;
                                default: break;
                            }
                            if (cbtex) { SDL_Rect cbdst = { ix - 20 + 1, iconY, 20, 20 }; SDL_RenderCopy(renderer, cbtex, NULL, &cbdst); ix -= 20 + 6; }
                        }
                        BulletBuffType bb = ch->get_active_bullet_buff();
                        SDL_Texture* btex = nullptr;
                        switch (bb) {
                            case BulletBuffType::BOUNCING: btex = rm.get_texture("bounce-buff"); break;
                            case BulletBuffType::EXPLODING: btex = rm.get_texture("explode-buff"); break;
                            case BulletBuffType::PIERCING: btex = rm.get_texture("piercing-buff"); break;
                            default: break;
                        }
                        if (btex) { SDL_Rect bdst = { ix - 20 + 1, iconY, 20, 20 }; SDL_RenderCopy(renderer, btex, NULL, &bdst); ix -= 20 + 6; }
                    }
                }

                // right column entries (team 1) - fixed 2 slots
                for (int ridx = 0; ridx < 2; ++ridx) {
                    Character* ch = team1_slots[ridx];
                    int x = panelRightX;
                    int y = panelY + ridx * entryH;

                    SDL_Rect entryBg = { x + 8, y + 8, panelW - 16, entryH - 16 };
                    SDL_SetRenderDrawColor(renderer, 24, 24, 24, 200);
                    SDL_RenderFillRect(renderer, &entryBg);

                    SDL_Rect sw = { x + 12, y + 12, 18, 18 };
                    SDL_SetRenderDrawColor(renderer, 80, 120, 220, 255);
                    SDL_RenderFillRect(renderer, &sw);

                    std::string basename = (ridx==0?"player2_1":"player2_2");
                    std::string fullName = basename + (ch ? (ch->get_gun_type() == GunType::AK ? " AK" : " PIS") : " (dead)");
                    int maxTextW = panelW - 28 - 48;
                    std::string nameToRender = ellipsize(fullName, maxTextW);
                    SDL_Color textColor = { 230, 230, 230, 255 };
                    int textH = 0;
                    if (ch) {
                        SDL_Surface* textSurf = TTF_RenderText_Blended(font, nameToRender.c_str(), textColor);
                        if (textSurf) {
                            textH = textSurf->h;
                            SDL_Texture* textTex = SDL_CreateTextureFromSurface(renderer, textSurf);
                            SDL_Rect dst = { x + 28, y + 10, textSurf->w, textH };
                            SDL_FreeSurface(textSurf);
                            if (textTex) { SDL_RenderCopy(renderer, textTex, NULL, &dst); SDL_DestroyTexture(textTex); }
                        }
                    } else {
                        SDL_Surface* textSurf = TTF_RenderText_Blended(font, nameToRender.c_str(), SDL_Color{160,160,160,255});
                        if (textSurf) {
                            textH = textSurf->h;
                            SDL_Texture* textTex = SDL_CreateTextureFromSurface(renderer, textSurf);
                            SDL_Rect dst = { x + 28, y + 10, textSurf->w, textH };
                            SDL_FreeSurface(textSurf);
                            if (textTex) { SDL_RenderCopy(renderer, textTex, NULL, &dst); SDL_DestroyTexture(textTex); }
                        }
                    }

                    float hp = ch ? std::max(0.0f, ch->get_health()) : 0.0f;
                    float hpfrac = std::min(1.0f, hp / 100.0f);
                    int hbW = 150;
                    int hbH = 12;
                    int hbY = y + 12 + textH + 8;
                    SDL_Rect hbBg = { x + 36, hbY, hbW, hbH };
                    SDL_SetRenderDrawColor(renderer, 60, 60, 60, 200); SDL_RenderFillRect(renderer, &hbBg);
                    SDL_Rect hbFg = { x + 36, hbY, (int)(hbW * hpfrac), hbH };
                    SDL_SetRenderDrawColor(renderer, (Uint8)(200 * (1.0f - hpfrac)), (Uint8)(200 * hpfrac), 50, 255); SDL_RenderFillRect(renderer, &hbFg);

                    int ix = x + panelW - 28;
                    int iconY = y + 10;
                    if (ch) {
                        auto cbs2 = ch->get_active_char_buffs();
                        for (auto cb : cbs2) {
                            SDL_Texture* cbtex = nullptr;
                            switch (cb) {
                                case CharBuffType::HEALTH: cbtex = rm.get_texture("health-buff"); break;
                                case CharBuffType::SPEED: cbtex = rm.get_texture("speed-buff"); break;
                                default: break;
                            }
                            if (cbtex) { SDL_Rect cbdst = { ix - 20 + 1, iconY, 20, 20 }; SDL_RenderCopy(renderer, cbtex, NULL, &cbdst); ix -= 20 + 6; }
                        }
                        BulletBuffType bb2 = ch->get_active_bullet_buff();
                        SDL_Texture* btex2 = nullptr;
                        switch (bb2) {
                            case BulletBuffType::BOUNCING: btex2 = rm.get_texture("bounce-buff"); break;
                            case BulletBuffType::EXPLODING: btex2 = rm.get_texture("explode-buff"); break;
                            case BulletBuffType::PIERCING: btex2 = rm.get_texture("piercing-buff"); break;
                            default: break;
                        }
                        if (btex2) { SDL_Rect bdst = { ix - 20 + 1, iconY, 20, 20 }; SDL_RenderCopy(renderer, btex2, NULL, &bdst); ix -= 20 + 6; }
                    }
                }
            }

            SDL_RenderPresent(renderer);
            SDL_Delay(16);
        }

    // If a winning team was determined, show a highlighted victory banner for 3 seconds
    if (winning_team == 1 || winning_team == 2) {
        Uint32 show_until = SDL_GetTicks() + 3000;
        while (SDL_GetTicks() < show_until) {
            SDL_Event e;
            while (SDL_PollEvent(&e)) {
                if (e.type == SDL_QUIT) { show_until = 0; break; }
            }
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 220);
            SDL_RenderClear(renderer);
            // semi-transparent highlight box
            std::string winText = "Team " + std::to_string(winning_team) + " wins!";
            SDL_Color hl = { 255, 200, 60, 255 };
            SDL_Color textColor = { 30, 30, 30, 255 };
            SDL_Surface* surf = TTF_RenderText_Blended(font ? font : nullptr, winText.c_str(), textColor);
            if (surf) {
                SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, surf);
                int tw = surf->w, th = surf->h;
                SDL_FreeSurface(surf);
                // draw highlighted box behind text
                SDL_Rect box = { WORLD_W/2 - (tw+40)/2, WORLD_H/2 - (th+30)/2, tw+40, th+30 };
                SDL_SetRenderDrawColor(renderer, hl.r, hl.g, hl.b, hl.a);
                SDL_RenderFillRect(renderer, &box);
                if (tex) {
                    SDL_Rect dst = { box.x + 20, box.y + 12, tw, th };
                    SDL_RenderCopy(renderer, tex, NULL, &dst);
                    SDL_DestroyTexture(tex);
                }
            }
            SDL_RenderPresent(renderer);
            SDL_Delay(16);
        }
    }

    // restore renderer logical size back to window for menu
    SDL_RenderSetLogicalSize(renderer, WINDOW_W, WINDOW_H);

    // cleanup textures we created
        SDL_DestroyTexture(red_texture);
        SDL_DestroyTexture(blue_texture);
    SDL_DestroyTexture(wall_tex_h);
    SDL_DestroyTexture(wall_tex_v);
    // cleanup random walls (PVP)
    for (auto* rw : pvp_random_walls) {
        if (rw) delete rw;
    }
    pvp_random_walls.clear();
    // cleanup buffs
    for (auto* bi : buffs) if (bi) delete bi;
    buffs.clear();
    // cleanup blackholes
    for (auto& bhp : blackholes) if (bhp.first) delete bhp.first;
    blackholes.clear();
        rm.unload_all();
    };

    // PVE runner: one player vs simple AI enemies
    auto run_pve_game = [&](void) {
        ResourceManager rm(renderer);
        rm.load_texture("bullet", "assets/pictures/bulletA.png");
        // battlefield background
        rm.load_texture("background", "assets/pictures/background.png");
        // PVE: preload buff textures so spawned buffs are visible
        rm.load_texture("health-buff", "assets/pictures/health-buff.png");
        rm.load_texture("speed-buff", "assets/pictures/speed-buff.png");
        rm.load_texture("bounce-buff", "assets/pictures/bounce-buff.png");
        rm.load_texture("explode-buff", "assets/pictures/explosion-buff.png");
        rm.load_texture("piercing-buff", "assets/pictures/piercing-buff.png");

        // Create four characters (two per team) so PVE mirrors PVP but with AI for the other team
        SDL_Surface* green_surface = SDL_CreateRGBSurface(0, 16, 16, 32,
            0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);
        SDL_FillRect(green_surface, NULL, SDL_MapRGBA(green_surface->format, 0, 255, 0, 255));
        SDL_Texture* green_texture = SDL_CreateTextureFromSurface(renderer, green_surface);
        SDL_FreeSurface(green_surface);

        SDL_Surface* red_surface = SDL_CreateRGBSurface(0, 16, 16, 32,
            0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);
        SDL_FillRect(red_surface, NULL, SDL_MapRGBA(red_surface->format, 255, 0, 0, 255));
        SDL_Texture* red_texture = SDL_CreateTextureFromSurface(renderer, red_surface);
        SDL_FreeSurface(red_surface);

        AnimatedSprite idle(renderer, "assets/pictures/PlayerIdle.png", 24, 16, 5, 100);
        AnimatedSprite run(renderer,  "assets/pictures/PlayerRunning.png", 24, 16, 5, 100);
        AnimatedSprite shoot(renderer,"assets/pictures/PlayerShooting.png", 24, 16, 5, 100);
        AnimatedSprite idle1(renderer, "assets/pictures/tocvangdung.png", 24, 16, 5, 100);
        AnimatedSprite run1(renderer,  "assets/pictures/tocvangchay.png", 24, 16, 5, 100);
        AnimatedSprite shoot1(renderer,"assets/pictures/tocvangban.png", 24, 16, 5, 100);

        // 1v1 PVE: one human player (p1) vs one AI (p3)
        Character p1(Vector2(WORLD_W/2.0f - 160.0f, WORLD_H - 120.0f), green_texture, 200.0f, 200.0f);
        // make AI slower: lower speed from 140 -> 90; give bot 200 health per request
        Character p3(Vector2(WORLD_W/2.0f + 160.0f - 40.0f, 120.0f), red_texture, 90.0f, 200.0f);

        p1.set_animations(&idle, &run, &shoot);
        p3.set_animations(&idle1, &run1, &shoot1);

    std::vector<Character*> characters = { &p1, &p3 };

    // Fixed slot mapping for HUD stability in PVE (1v1)
    std::unordered_map<Character*, int> pve_slot_index;
    pve_slot_index[&p1] = 0;
    pve_slot_index[&p3] = 1;

    std::vector<Bullet*> bullets;
    // PVE: local random walls container (pve_random_walls)
    std::vector<Wall*> pve_random_walls;
    // PVE blackholes container is declared later as pve_blackholes_local
    std::vector<Explosion*> explosions;
    std::vector<BloodSplash*> bloods;
    std::vector<Smoke*> smokes;
    // Buff items for PVE
    std::vector<BuffItem*> buffs;
    const Uint32 buff_interval_ms_pve = 10000; // spawn every 10s in PVE
    // force a first immediate spawn
    Uint32 last_buff_spawn_pve = SDL_GetTicks() - buff_interval_ms_pve;
    // previous health tracking for death detection
    std::unordered_map<Character*, float> prev_health;
    for (auto c : characters) if (c) prev_health[c] = c->get_health();

    // Ensure distinct teams/input sets so bullets are treated as enemies
    p1.set_input_set((int)InputSet::INPUT_1);
    p1.set_activate(true);
    p3.set_input_set((int)InputSet::INPUT_2);
    p3.set_activate(false);

    // BasicAI controller will control the enemy (p3), human controls p1
    BasicAI ai1(&p3, &p1, &bullets, &rm); // ai targets player

    // Input handler for the human player controlling p1 only
    InputHandler ih_player(InputSet::INPUT_1, &p1, nullptr);

    // Simple updatables list (player, enemy, input handler, AI)
    std::vector<IUpdatable*> updatables;
    updatables.push_back(&p1);
    updatables.push_back(&p3);
    updatables.push_back(&ih_player);
    updatables.push_back(&ai1);

        // Gun-change timer for PVE: rotate guns every 30s
        const Uint32 gun_change_ms = 30000;
        Uint32 last_gun_change = SDL_GetTicks();

        // Walls (reuse same wall creation as PVP for bounds)
        const int wall_thickness = 32;
        SDL_Surface* wall_surf_h = SDL_CreateRGBSurface(0, WORLD_W, wall_thickness, 32, 0x00FF0000,0x0000FF00,0x000000FF,0xFF000000);
        SDL_FillRect(wall_surf_h, NULL, SDL_MapRGBA(wall_surf_h->format, 80, 80, 80, 255));
        SDL_Texture* wall_tex_h = SDL_CreateTextureFromSurface(renderer, wall_surf_h);
        SDL_FreeSurface(wall_surf_h);
        SDL_Surface* wall_surf_v = SDL_CreateRGBSurface(0, wall_thickness, WORLD_H, 32, 0x00FF0000,0x0000FF00,0x000000FF,0xFF000000);
        SDL_FillRect(wall_surf_v, NULL, SDL_MapRGBA(wall_surf_v->format, 80, 80, 80, 255));
        SDL_Texture* wall_tex_v = SDL_CreateTextureFromSurface(renderer, wall_surf_v);
        SDL_FreeSurface(wall_surf_v);
        Wall topWall(Vector2(WORLD_W/2.0f, wall_thickness / 2.0f), wall_tex_h);
        Wall bottomWall(Vector2(WORLD_W/2.0f, WORLD_H - wall_thickness / 2.0f), wall_tex_h);
        Wall leftWall(Vector2(wall_thickness / 2.0f, WORLD_H/2.0f), wall_tex_v);
        Wall rightWall(Vector2(WORLD_W - wall_thickness / 2.0f, WORLD_H/2.0f), wall_tex_v);

    // RNG and random internal walls for PVE (mirror PVP behavior)
    std::random_device rd_pve;
    std::mt19937 rng_pve(rd_pve());
    std::uniform_real_distribution<float> wallX_pve(150.0f, WORLD_W - 150.0f);
    std::uniform_real_distribution<float> wallY_pve(150.0f, WORLD_H - 150.0f);
    std::uniform_int_distribution<int> wallW_pve(64, 240);
    std::uniform_int_distribution<int> wallH_pve(16, 96);
    for (int i = 0; i < 7; ++i) {
        int w = wallW_pve(rng_pve);
        int h = wallH_pve(rng_pve);
        bool placed = false;
        int attempts = 0;
        Vector2 chosenPos;
        while (!placed && attempts < 30) {
            Vector2 pos(wallX_pve(rng_pve), wallY_pve(rng_pve));
            OBB tmp_box(pos, Vector2(w / 2.0f, h / 2.0f), 0.0f);
            bool intersects = false;
            for (auto* hb : topWall.get_hitboxes()) if (hb->is_collide(tmp_box)) { intersects = true; break; }
            if (intersects) { attempts++; continue; }
            for (auto* hb : bottomWall.get_hitboxes()) if (hb->is_collide(tmp_box)) { intersects = true; break; }
            if (intersects) { attempts++; continue; }
            for (auto* hb : leftWall.get_hitboxes()) if (hb->is_collide(tmp_box)) { intersects = true; break; }
            if (intersects) { attempts++; continue; }
            for (auto* hb : rightWall.get_hitboxes()) if (hb->is_collide(tmp_box)) { intersects = true; break; }
            if (intersects) { attempts++; continue; }
            for (auto* existing : pve_random_walls) if (existing) {
                for (auto* hb : existing->get_hitboxes()) {
                    if (hb->is_collide(tmp_box)) { intersects = true; break; }
                }
                if (intersects) break;
            }
            if (!intersects) {
                const float min_player_clearance = 150.0f;
                for (auto* pc : characters) if (pc) {
                    float dx = pc->get_position().x - pos.x;
                    float dy = pc->get_position().y - pos.y;
                    if (dx*dx + dy*dy < min_player_clearance * min_player_clearance) { intersects = true; break; }
                }
            }
            if (!intersects) { placed = true; chosenPos = pos; } else attempts++;
        }
        if (!placed) continue;
        SDL_Surface* surf = SDL_CreateRGBSurface(0, w, h, 32, 0x00FF0000,0x0000FF00,0x000000FF,0xFF000000);
        SDL_FillRect(surf, NULL, SDL_MapRGBA(surf->format, 100, 100, 100, 255));
        SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, surf);
        SDL_FreeSurface(surf);
        Wall* rw = new Wall(chosenPos, tex);
        pve_random_walls.push_back(rw);
        updatables.push_back(rw);
    }

    // game loop simple
    SDL_RenderSetLogicalSize(renderer, WORLD_W, WORLD_H);
    bool in_game = true;
    Uint32 last = SDL_GetTicks();
    struct Notify { std::string text; Uint32 expiry; };
    std::vector<Notify> notifications;
    // pve_result: 1 = player win, -1 = player lose, 0 = none
    int pve_result = 0;
    // Blackhole RNG + timing for PVE
    std::random_device pve_rd_bh;
    std::mt19937 pve_rng_bh(pve_rd_bh());
    std::uniform_real_distribution<float> pve_distX(100.0f, WORLD_W - 100.0f);
    std::uniform_real_distribution<float> pve_distY(100.0f, WORLD_H - 100.0f);
    const Uint32 pve_blackhole_precaution_ms = 5000;
    const Uint32 pve_blackhole_interval_ms = 30000;
    const Uint32 pve_blackhole_life_ms = 15000;
    Uint32 pve_start_time = SDL_GetTicks();
    Uint32 pve_last_bh_spawn = 0;
    // PVE blackholes container
    std::vector<std::pair<BlackHole*, Uint32>> pve_blackholes_local;
    // Generate random internal walls for PVE (similar to PVP)
    {
        std::random_device rdp;
        std::mt19937 rngp(rdp());
        std::uniform_real_distribution<float> wallXp(150.0f, WORLD_W - 150.0f);
        std::uniform_real_distribution<float> wallYp(150.0f, WORLD_H - 150.0f);
        std::uniform_int_distribution<int> wallWp(64, 240);
        std::uniform_int_distribution<int> wallHp(16, 96);
        for (int i = 0; i < 7; ++i) {
            int w = wallWp(rngp);
            int h = wallHp(rngp);
            bool placed = false;
            int attempts = 0;
            Vector2 chosenPos;
            while (!placed && attempts < 30) {
                Vector2 pos(wallXp(rngp), wallYp(rngp));
                OBB tmp_box(pos, Vector2(w / 2.0f, h / 2.0f), 0.0f);
                bool intersects = false;
                for (auto* hb : topWall.get_hitboxes()) if (hb->is_collide(tmp_box)) { intersects = true; break; }
                if (intersects) { attempts++; continue; }
                for (auto* hb : bottomWall.get_hitboxes()) if (hb->is_collide(tmp_box)) { intersects = true; break; }
                if (intersects) { attempts++; continue; }
                for (auto* hb : leftWall.get_hitboxes()) if (hb->is_collide(tmp_box)) { intersects = true; break; }
                if (intersects) { attempts++; continue; }
                for (auto* hb : rightWall.get_hitboxes()) if (hb->is_collide(tmp_box)) { intersects = true; break; }
                if (intersects) { attempts++; continue; }
                for (auto* existing : pve_random_walls) if (existing) {
                    for (auto* hb : existing->get_hitboxes()) { if (hb->is_collide(tmp_box)) { intersects = true; break; } }
                    if (intersects) break;
                }
                if (!intersects) {
                    const float min_player_clearance = 150.0f;
                    for (auto* pc : characters) if (pc) {
                        float dx = pc->get_position().x - pos.x;
                        float dy = pc->get_position().y - pos.y;
                        if (dx*dx + dy*dy < min_player_clearance * min_player_clearance) { intersects = true; break; }
                    }
                }
                if (!intersects) { placed = true; chosenPos = pos; } else attempts++;
            }
            if (!placed) continue;
            SDL_Surface* surf = SDL_CreateRGBSurface(0, w, h, 32, 0x00FF0000,0x0000FF00,0x000000FF,0xFF000000);
            SDL_FillRect(surf, NULL, SDL_MapRGBA(surf->format, 100, 100, 100, 255));
            SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, surf);
            SDL_FreeSurface(surf);
            Wall* rw = new Wall(chosenPos, tex);
            pve_random_walls.push_back(rw);
            updatables.push_back(rw);
        }
    }
    AnimatedSprite pve_blackhole_anim(renderer, "assets/pictures/output.png", 200, 200, 12, 100, 3);
        while (in_game) {
            SDL_Event e;
            while (SDL_PollEvent(&e)) {
                if (e.type == SDL_QUIT) { in_game = false; break; }
                if (e.type == SDL_KEYDOWN && e.key.keysym.scancode == SDL_SCANCODE_ESCAPE) { in_game = false; break; }
                if (e.type == SDL_KEYDOWN && e.key.keysym.scancode == SDL_SCANCODE_E) {
                    Vector2 pos(WORLD_W/2.0f - 32.0f, WORLD_H/2.0f - 32.0f);
                    Smoke* s = new Smoke(renderer, "assets/pictures/khoi.png", pos, 24, 24, 8, 80, 3);
                    smokes.push_back(s);
                }
                // forward events to player input handler
                ih_player.handle_event(e, bullets, rm);
            }
            Uint32 now = SDL_GetTicks();
            float dt = (now - last) / 1000.0f; last = now;

            // update
            for (auto* u : updatables) u->update(dt);
            for (auto& bhp : pve_blackholes_local) if (bhp.first) bhp.first->update(dt);
            // Update PVE blackholes (local container)
            for (auto& bhp : pve_blackholes_local) if (bhp.first) bhp.first->update(dt);
            for (auto* b : bullets) b->update(dt);
            for (auto* ex : explosions) ex->update(dt);
            // Explosion collisions in PVE: characters/bullets
            for (auto* ex : explosions) {
                for (auto c : characters) if (c) c->collide(ex);
                for (auto* bl : bullets) if (bl) ex->collide(bl);
            }
            for (auto* b : bloods) b->update(dt);
            for (auto* s : smokes) s->update(dt);

            // Rotate guns every gun_change_ms in PVE (toggle all characters, same as PVP)
            if (SDL_GetTicks() - last_gun_change >= gun_change_ms) {
                last_gun_change = SDL_GetTicks();
                for (auto* c : characters) {
                    if (!c) continue;
                    GunType cur = c->get_gun_type();
                    GunType next = GunType::PISTOL;
                    if (cur == GunType::PISTOL) next = GunType::AK;
                    else next = GunType::PISTOL;
                    c->set_gun_type(next);
                }
                notifications.push_back({ std::string("Guns switched!"), SDL_GetTicks() + 2500 });
            }

            // Buff spawn logic for PVE (every buff_interval_ms_pve)
            if (SDL_GetTicks() - last_buff_spawn_pve >= buff_interval_ms_pve) {
                // pick a random buff type (char buff or bullet buff) - reuse simple selection
                std::variant<CharBuffType, BulletBuffType> bt;
                // check if any player currently has a bullet buff
                bool any_bullet_buff = false;
                for (auto* c : characters) if (c) if (c->get_gun_buff_type() != BulletBuffType::NONE) { any_bullet_buff = true; break; }
                std::uniform_int_distribution<int> chooseType(0, 1);
                int kind = chooseType(rng_pve);
                if (kind == 0) {
                    std::uniform_int_distribution<int> chooseChar(0, (int)CharBuffType::NUM - 1);
                    bt = static_cast<CharBuffType>(chooseChar(rng_pve));
                } else {
                    if (any_bullet_buff) {
                        std::uniform_int_distribution<int> chooseChar(0, (int)CharBuffType::NUM - 1);
                        bt = static_cast<CharBuffType>(chooseChar(rng_pve));
                    } else {
                        std::vector<BulletBuffType> bulletTypes = { BulletBuffType::BOUNCING, BulletBuffType::EXPLODING, BulletBuffType::PIERCING };
                        std::uniform_int_distribution<int> chooseBullet(0, (int)bulletTypes.size() - 1);
                        bt = bulletTypes[chooseBullet(rng_pve)];
                    }
                }
                // pick a spawn position avoiding walls and players
                Vector2 pos;
                bool placed = false;
                for (int attempt = 0; attempt < 30 && !placed; ++attempt) {
                    pos = Vector2(wallX_pve(rng_pve), wallY_pve(rng_pve));
                    OBB tmp_box(pos, Vector2(16.0f, 16.0f), 0.0f);
                    bool intersects = false;
                    for (auto* hb : topWall.get_hitboxes()) if (hb->is_collide(tmp_box)) { intersects = true; break; }
                    if (intersects) continue;
                    for (auto* hb : bottomWall.get_hitboxes()) if (hb->is_collide(tmp_box)) { intersects = true; break; }
                    if (intersects) continue;
                    for (auto* hb : leftWall.get_hitboxes()) if (hb->is_collide(tmp_box)) { intersects = true; break; }
                    if (intersects) continue;
                    for (auto* hb : rightWall.get_hitboxes()) if (hb->is_collide(tmp_box)) { intersects = true; break; }
                    if (intersects) continue;
                    for (auto* rw : pve_random_walls) if (rw) {
                        for (auto* hb : rw->get_hitboxes()) if (hb->is_collide(tmp_box)) { intersects = true; break; }
                        if (intersects) break;
                    }
                    if (intersects) continue;
                    // avoid players
                    const float min_clear = 120.0f;
                    for (auto* pc : characters) if (pc) {
                        float dx = pc->get_position().x - pos.x;
                        float dy = pc->get_position().y - pos.y;
                        if (dx*dx + dy*dy < min_clear * min_clear) { intersects = true; break; }
                    }
                    if (!intersects) placed = true;
                }
                if (placed) {
                    SDL_Texture* chosen_tex = nullptr;
                    if (std::holds_alternative<CharBuffType>(bt)) {
                        CharBuffType cb = std::get<CharBuffType>(bt);
                        switch (cb) {
                            case CharBuffType::HEALTH: chosen_tex = rm.get_texture("health-buff"); break;
                            case CharBuffType::SPEED: chosen_tex = rm.get_texture("speed-buff"); break;
                            default: chosen_tex = rm.get_texture("health-buff"); break;
                        }
                    } else {
                        BulletBuffType bb = std::get<BulletBuffType>(bt);
                        switch (bb) {
                            case BulletBuffType::BOUNCING: chosen_tex = rm.get_texture("bounce-buff"); break;
                            case BulletBuffType::EXPLODING: chosen_tex = rm.get_texture("explode-buff"); break;
                            case BulletBuffType::PIERCING: chosen_tex = rm.get_texture("piercing-buff"); break;
                            default: chosen_tex = nullptr; break;
                        }
                    }
                    SDL_Texture* btex = chosen_tex;
                    if (!btex) {
                        SDL_Surface* bs = SDL_CreateRGBSurface(0, 32, 32, 32, 0x00FF0000,0x0000FF00,0x000000FF,0xFF000000);
                        SDL_FillRect(bs, NULL, SDL_MapRGBA(bs->format, 200, 100, 0, 255));
                        btex = SDL_CreateTextureFromSurface(renderer, bs);
                        SDL_FreeSurface(bs);
                    }
                    BuffItem* bi = new BuffItem(pos, btex, bt);
                    buffs.push_back(bi);
                    updatables.push_back(bi);
                }
                last_buff_spawn_pve = SDL_GetTicks();
            }

            // PVE Blackhole spawn & lifetime logic (mirrors PVP)
            
            Uint32 now_pve_bh = SDL_GetTicks();
            if (now_pve_bh - pve_start_time >= pve_blackhole_precaution_ms) {
                if (pve_last_bh_spawn == 0 || now_pve_bh - pve_last_bh_spawn >= pve_blackhole_interval_ms) {
                    bool ok = false; int attempts = 0; Vector2 p;
                    while (!ok && attempts < 20) {
                        p.x = pve_distX(pve_rng_bh);
                        p.y = pve_distY(pve_rng_bh);
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
                        
                        nb->set_animation(&pve_blackhole_anim);
                        pve_blackholes_local.emplace_back(nb, now_pve_bh);
                    }
                    pve_last_bh_spawn = now_pve_bh;
                }
            }

            // Remove expired PVE blackholes
            for (auto& bhp : pve_blackholes_local) {
                if (bhp.first && now_pve_bh - bhp.second >= pve_blackhole_life_ms) {
                    delete bhp.first;
                    bhp.first = nullptr;
                }
            }
            pve_blackholes_local.erase(std::remove_if(pve_blackholes_local.begin(), pve_blackholes_local.end(), [](const std::pair<BlackHole*,Uint32>& p){ return p.first == nullptr; }), pve_blackholes_local.end());

            // let characters pick up buffs
            for (auto* c : characters) if (c) for (auto* bi : buffs) if (bi) c->collide(bi);
            // remove consumed buffs
            {
                auto it = buffs.begin();
                while (it != buffs.end()) {
                    BuffItem* bi = *it;
                    if (bi->is_consumed()) {
                        // If the consumed buff was a BulletBuff, clear bullet buffs from other characters
                        auto btype = bi->get_buff_type();
                        if (std::holds_alternative<BulletBuffType>(btype)) {
                            BulletBuffType taken = std::get<BulletBuffType>(btype);
                            for (auto* c : characters) if (c) {
                                if (c->get_gun_buff_type() != BulletBuffType::NONE && c->get_gun_buff_type() != taken) {
                                    c->clear_bullet_buff();
                                }
                            }
                        }
                        updatables.erase(std::remove(updatables.begin(), updatables.end(), static_cast<IUpdatable*>(bi)), updatables.end());
                        delete bi;
                        it = buffs.erase(it);
                    } else ++it;
                }
            }

            // detect damage and deaths in PVE: spawn blood on hit, smoke on death, remove dead
            std::vector<Character*> pve_just_died;
            for (auto c : characters) {
                if (!c) continue;
                float old_h = prev_health[c];
                float new_h = c->get_health();
                if (new_h < old_h && new_h > 0.0f) {
                    Vector2 bpos = c->get_position();
                    BloodSplash* bs = new BloodSplash(renderer, std::string("assets/pictures/blood.png"), bpos, 24, 24, 8, 80, 3);
                    bloods.push_back(bs);
                }
                if (old_h > 0.0f && new_h <= 0.0f) {
                    Vector2 spos = c->get_position();
                    Smoke* s = new Smoke(renderer, "assets/pictures/khoi.png", spos, 24, 24, 8, 80, 3);
                    smokes.push_back(s);
                    SDL_Log("PVE Spawned Smoke at %.1f, %.1f", spos.x, spos.y);
                    pve_just_died.push_back(c);
                    ih_player.on_character_death(c);
                }
                prev_health[c] = new_h;
            }
            if (!pve_just_died.empty()) {
                bool ai_died = false;
                for (auto d : pve_just_died) {
                    if (d == &p3) ai_died = true;
                    characters.erase(std::remove(characters.begin(), characters.end(), d), characters.end());
                    updatables.erase(std::remove(updatables.begin(), updatables.end(), static_cast<IUpdatable*>(d)), updatables.end());
                    prev_health.erase(d);
                }
                // End the PVE match when any character dies. Notify the player of win/lose.
                if (ai_died) {
                    notifications.push_back({ std::string("You win"), SDL_GetTicks() + 3000 });
                    pve_result = 1;
                } else {
                    notifications.push_back({ std::string("You lose"), SDL_GetTicks() + 3000 });
                    pve_result = -1;
                }
                in_game = false;
            }

            // cleanup finished explosions
            explosions.erase(std::remove_if(explosions.begin(), explosions.end(), [](Explosion* ex){ if (ex->is_finished()) { delete ex; return true; } return false; }), explosions.end());
            // cleanup finished bloods
            bloods.erase(std::remove_if(bloods.begin(), bloods.end(), [](BloodSplash* b){ if (b->is_finished()) { delete b; return true; } return false; }), bloods.end());
            // cleanup finished smokes
            smokes.erase(std::remove_if(smokes.begin(), smokes.end(), [](Smoke* s){ if (s->is_finished()) { delete s; return true; } return false; }), smokes.end());

            // collisions: bullets vs blackholes/characters/walls
            for (auto* b : bullets) {
                if (!b) continue;
                // blackholes first
                for (auto& bhp : pve_blackholes_local) if (bhp.first) bhp.first->collide(b);
                // PVE blackholes collide with bullets
                for (auto& bhp : pve_blackholes_local) if (bhp.first) bhp.first->collide(b);
                if (b->is_destroyed()) continue;
                // Characters absorb bullet collisions
                for (auto* ch : characters) if (ch) ch->collide(b);
                topWall.collide(b); if (b->is_destroyed()) continue;
                bottomWall.collide(b); if (b->is_destroyed()) continue;
                leftWall.collide(b); if (b->is_destroyed()) continue;
                rightWall.collide(b); if (b->is_destroyed()) continue;
                for (auto* rw : pve_random_walls) if (rw) { rw->collide(b); if (b->is_destroyed()) break; }
            }
            // bullet vs bullet collisions in PVE: bullets from different teams (if any) destroy each other
            for (size_t i = 0; i < bullets.size(); ++i) {
                Bullet* a = bullets[i];
                if (!a || a->is_destroyed()) continue;
                for (size_t j = i + 1; j < bullets.size(); ++j) {
                    Bullet* b = bullets[j];
                    if (!b || b->is_destroyed()) continue;
                    if (a->get_team_id() == b->get_team_id()) continue;
                    bool collided = false;
                    for (auto* ah : a->get_hitboxes()) {
                        for (auto* bh : b->get_hitboxes()) {
                            if (ah->is_collide(*bh)) { collided = true; break; }
                        }
                        if (collided) break;
                    }
                    if (collided) {
                        a->set_destroyed(true);
                        b->set_destroyed(true);
                    }
                }
            }
            // remove destroyed bullets
            bullets.erase(std::remove_if(bullets.begin(), bullets.end(), [&](Bullet* bullet){
                if (!bullet) return true;
                if (bullet->is_destroyed()) {
                    if (bullet->getBuff() == BulletBuffType::EXPLODING) {
                        bullet->explode(explosions, renderer);
                    }
                    delete bullet;
                    return true;
                }
                return false;
            }), bullets.end());

            // Ensure characters collide with boundary walls, blackholes and random internal walls so they cannot pass through
            for (auto* c : characters) {
                if (!c) continue;
                topWall.collide(c);
                bottomWall.collide(c);
                leftWall.collide(c);
                rightWall.collide(c);
                for (auto& bhp : pve_blackholes_local) if (bhp.first) bhp.first->collide(c);
                // PVE blackholes collide with characters
                for (auto& bhp : pve_blackholes_local) if (bhp.first) bhp.first->collide(c);
                for (auto* rw : pve_random_walls) if (rw) rw->collide(c);
            }

            // render
            SDL_SetRenderDrawColor(renderer, 0,0,0,255);
            SDL_RenderClear(renderer);
            // draw battlefield background (if available)
            SDL_Texture* bgtex = rm.get_texture("background");
            if (bgtex) {
                SDL_Rect dst = { 0, 0, WORLD_W, WORLD_H };
                SDL_RenderCopy(renderer, bgtex, NULL, &dst);
            }
            SDL_Rect worldRect = {0,0,WORLD_W,WORLD_H};
            SDL_SetRenderDrawColor(renderer, 0xFF, 0x00, 0x00, 0xFF);
            SDL_RenderDrawRect(renderer, &worldRect);
            topWall.render(renderer); bottomWall.render(renderer); leftWall.render(renderer); rightWall.render(renderer);
            for (auto* rw : pve_random_walls) if (rw) rw->render(renderer);
            for (auto* ch : characters) if (ch) ch->render(renderer);
            for (auto* b : bullets) b->render(renderer);
            for (auto* ex : explosions) { ex->render(renderer); }
            for (auto* bi : buffs) if (bi) bi->render(renderer);
            for (auto* bl : bloods) bl->render(renderer);
            for (auto* s : smokes) s->render(renderer);
            for (auto& bhp : pve_blackholes_local) if (bhp.first) bhp.first->render(renderer);
            for (auto& bhp : pve_blackholes_local) if (bhp.first) bhp.first->render(renderer);
            // simple HUD for PVE: show player health + bullet buff icon
            if (font) {
                SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
                // render notifications (centered on screen)
                int validCount = 0;
                for (auto &n : notifications) if ((int)n.text.size() != 0 && SDL_GetTicks() <= n.expiry) ++validCount;
                int lineH = 20;
                int notifY = WORLD_H/2 - (validCount * lineH) / 2;
                for (auto it = notifications.begin(); it != notifications.end();) {
                    if ((int)it->text.size() == 0 || SDL_GetTicks() > it->expiry) { it = notifications.erase(it); continue; }
                    SDL_Color textColor = { 255, 220, 120, 255 };
                    SDL_Surface* t = TTF_RenderText_Blended(font, it->text.c_str(), textColor);
                    if (t) {
                        SDL_Texture* tt = SDL_CreateTextureFromSurface(renderer, t);
                        int tw = t->w, th = t->h;
                        SDL_Rect td = { WORLD_W/2 - tw/2, notifY, tw, th };
                        SDL_FreeSurface(t);
                        if (tt) { SDL_RenderCopy(renderer, tt, NULL, &td); SDL_DestroyTexture(tt); }
                    }
                    notifY += lineH;
                    ++it;
                }
                // use the same split HUD as PVP for PVE so both modes match visually

                int players = (int)characters.size();
                int left_count = (players + 1) / 2;
                int right_count = players - left_count;
                const int panelW = 270; // smaller panel width
                const int entryH = 56;  // reduced entry height
                const int panelY = 8;

                // left panel background
                int panelHLeft = 16 + left_count * entryH;
                int panelLeftX = 8;
                SDL_Rect panelLeftBg = { panelLeftX - 6, panelY - 6, panelW, panelHLeft };
                SDL_SetRenderDrawColor(renderer, 0, 0, 0, 160);
                SDL_RenderFillRect(renderer, &panelLeftBg);

                // right panel background
                int panelHRight = 16 + right_count * entryH;
                int panelRightX = WORLD_W - panelW - 8;
                SDL_Rect panelRightBg = { panelRightX - 6, panelY - 6, panelW, panelHRight };
                SDL_SetRenderDrawColor(renderer, 0, 0, 0, 160);
                SDL_RenderFillRect(renderer, &panelRightBg);

                // helper: ellipsize to pixel width
                auto ellipsize = [&](const std::string &full, int maxW) {
                    std::string s = full;
                    int w = 0, h = 0;
                    if (TTF_SizeText(font, s.c_str(), &w, &h) == 0 && w <= maxW) return s;
                    while (!s.empty()) {
                        s = s.substr(0, s.size() - 1);
                        std::string t = s + "...";
                        if (TTF_SizeText(font, t.c_str(), &w, &h) == 0 && w <= maxW) return t;
                    }
                    return std::string("...");
                };

                // left column entries
                for (int idx = 0; idx < left_count; ++idx) {
                    int i = idx;
                    Character* ch = characters[i];
                    if (!ch) continue;
                    int x = panelLeftX;
                    int y = panelY + idx * entryH;

                    SDL_Rect entryBg = { x + 8, y + 8, panelW - 16, entryH - 16 };
                    SDL_SetRenderDrawColor(renderer, 24, 24, 24, 200);
                    SDL_RenderFillRect(renderer, &entryBg);

                    SDL_Rect sw = { x + 12, y + 12, 18, 18 };
                    if (i < 2) SDL_SetRenderDrawColor(renderer, 200, 60, 60, 255); else SDL_SetRenderDrawColor(renderer, 80, 120, 220, 255);
                    SDL_RenderFillRect(renderer, &sw);

                    std::string basename = (i==0?"player1_1":(i==1?"player1_2":(i==2?"player2_1":(i==3?"player2_2":"player"+std::to_string(i+1)))));
                    GunType gt = ch->get_gun_type();
                    std::string fullName = basename + (gt == GunType::AK ? " AK" : " PIS");
                    int maxTextW = panelW - 28 - 48;
                    std::string nameToRender = ellipsize(fullName, maxTextW);
                    SDL_Color textColor = { 230, 230, 230, 255 };
                    int textH = 0;
                    SDL_Surface* textSurf = TTF_RenderText_Blended(font, nameToRender.c_str(), textColor);
                    if (textSurf) {
                        textH = textSurf->h;
                        SDL_Texture* textTex = SDL_CreateTextureFromSurface(renderer, textSurf);
                        SDL_Rect dst = { x + 28, y + 10, textSurf->w, textH };
                        SDL_FreeSurface(textSurf);
                        if (textTex) { SDL_RenderCopy(renderer, textTex, NULL, &dst); SDL_DestroyTexture(textTex); }
                    }

                    float hp = std::max(0.0f, ch->get_health());
                    float hpfrac = std::min(1.0f, hp / 100.0f);
                    int hbW = 150;
                    int hbH = 12;
                    int hbY = y + 12 + textH + 8;
                    SDL_Rect hbBg = { x + 36, hbY, hbW, hbH };
                    SDL_SetRenderDrawColor(renderer, 60, 60, 60, 200); SDL_RenderFillRect(renderer, &hbBg);
                    SDL_Rect hbFg = { x + 36, hbY, (int)(hbW * hpfrac), hbH };
                    SDL_SetRenderDrawColor(renderer, (Uint8)(200 * (1.0f - hpfrac)), (Uint8)(200 * hpfrac), 50, 255); SDL_RenderFillRect(renderer, &hbFg);

                    int ix = x + panelW - 28;
                    int iconY = y + 10;
                    auto cbs = ch->get_active_char_buffs();
                    for (auto cb : cbs) {
                        SDL_Texture* cbtex = nullptr;
                        switch (cb) {
                            case CharBuffType::HEALTH: cbtex = rm.get_texture("health-buff"); break;
                            case CharBuffType::SPEED: cbtex = rm.get_texture("speed-buff"); break;
                            default: break;
                        }
                        if (cbtex) { SDL_Rect cbdst = { ix - 20 + 1, iconY, 20, 20 }; SDL_RenderCopy(renderer, cbtex, NULL, &cbdst); ix -= 20 + 6; }
                    }
                    BulletBuffType bb = ch->get_active_bullet_buff();
                    SDL_Texture* btex = nullptr;
                    switch (bb) {
                        case BulletBuffType::BOUNCING: btex = rm.get_texture("bounce-buff"); break;
                        case BulletBuffType::EXPLODING: btex = rm.get_texture("explode-buff"); break;
                        case BulletBuffType::PIERCING: btex = rm.get_texture("piercing-buff"); break;
                        default: break;
                    }
                    if (btex) { SDL_Rect bdst = { ix - 20 + 1, iconY, 20, 20 }; SDL_RenderCopy(renderer, btex, NULL, &bdst); ix -= 20 + 6; }
                }

                // right column entries
                for (int ridx = 0; ridx < right_count; ++ridx) {
                    int i = left_count + ridx;
                    Character* ch = characters[i];
                    if (!ch) continue;
                    int x = panelRightX;
                    int y = panelY + ridx * entryH;

                    SDL_Rect entryBg = { x + 8, y + 8, panelW - 16, entryH - 16 };
                    SDL_SetRenderDrawColor(renderer, 24, 24, 24, 200);
                    SDL_RenderFillRect(renderer, &entryBg);

                    SDL_Rect sw = { x + 12, y + 12, 18, 18 };
                    if (i < 2) SDL_SetRenderDrawColor(renderer, 200, 60, 60, 255); else SDL_SetRenderDrawColor(renderer, 80, 120, 220, 255);
                    SDL_RenderFillRect(renderer, &sw);

                    std::string basename = (i==0?"player1_1":(i==1?"player1_2":(i==2?"player2_1":(i==3?"player2_2":"player"+std::to_string(i+1)))));
                    GunType gt = ch->get_gun_type();
                    std::string fullName = basename + (gt == GunType::AK ? " AK" : " PIS");
                    int maxTextW = panelW - 28 - 48;
                    std::string nameToRender = ellipsize(fullName, maxTextW);
                    SDL_Color textColor = { 230, 230, 230, 255 };
                    int textH = 0;
                    SDL_Surface* textSurf = TTF_RenderText_Blended(font, nameToRender.c_str(), textColor);
                    if (textSurf) {
                        textH = textSurf->h;
                        SDL_Texture* textTex = SDL_CreateTextureFromSurface(renderer, textSurf);
                        SDL_Rect dst = { x + 28, y + 10, textSurf->w, textH };
                        SDL_FreeSurface(textSurf);
                        if (textTex) { SDL_RenderCopy(renderer, textTex, NULL, &dst); SDL_DestroyTexture(textTex); }
                    }

                    float hp = std::max(0.0f, ch->get_health());
                    float hpfrac = std::min(1.0f, hp / 100.0f);
                    int hbW = 150;
                    int hbH = 12;
                    int hbY = y + 12 + textH + 8;
                    SDL_Rect hbBg = { x + 36, hbY, hbW, hbH };
                    SDL_SetRenderDrawColor(renderer, 60, 60, 60, 200); SDL_RenderFillRect(renderer, &hbBg);
                    SDL_Rect hbFg = { x + 36, hbY, (int)(hbW * hpfrac), hbH };
                    SDL_SetRenderDrawColor(renderer, (Uint8)(200 * (1.0f - hpfrac)), (Uint8)(200 * hpfrac), 50, 255); SDL_RenderFillRect(renderer, &hbFg);

                    int ix = x + panelW - 28;
                    int iconY = y + 10;
                    auto cbs2 = ch->get_active_char_buffs();
                    for (auto cb : cbs2) {
                        SDL_Texture* cbtex = nullptr;
                        switch (cb) {
                            case CharBuffType::HEALTH: cbtex = rm.get_texture("health-buff"); break;
                            case CharBuffType::SPEED: cbtex = rm.get_texture("speed-buff"); break;
                            default: break;
                        }
                        if (cbtex) { SDL_Rect cbdst = { ix - 20 + 1, iconY, 20, 20 }; SDL_RenderCopy(renderer, cbtex, NULL, &cbdst); ix -= 20 + 6; }
                    }
                    BulletBuffType bb2 = ch->get_active_bullet_buff();
                    SDL_Texture* btex2 = nullptr;
                    switch (bb2) {
                        case BulletBuffType::BOUNCING: btex2 = rm.get_texture("bounce-buff"); break;
                        case BulletBuffType::EXPLODING: btex2 = rm.get_texture("explode-buff"); break;
                        case BulletBuffType::PIERCING: btex2 = rm.get_texture("piercing-buff"); break;
                        default: break;
                    }
                    if (btex2) { SDL_Rect bdst = { ix - 20 + 1, iconY, 20, 20 }; SDL_RenderCopy(renderer, btex2, NULL, &bdst); ix -= 20 + 6; }
                }
            }
            SDL_RenderPresent(renderer);
            SDL_Delay(16);
        }

        SDL_RenderSetLogicalSize(renderer, WINDOW_W, WINDOW_H);
        // If PVE produced a result, show a centered banner for 3 seconds so the player sees the outcome
        if (pve_result != 0) {
            std::string msg = (pve_result == 1) ? "You win" : "You lose";
            Uint32 show_until = SDL_GetTicks() + 3000;
            while (SDL_GetTicks() < show_until) {
                SDL_Event e;
                while (SDL_PollEvent(&e)) {
                    if (e.type == SDL_QUIT) { show_until = 0; break; }
                }
                SDL_SetRenderDrawColor(renderer, 0, 0, 0, 220);
                SDL_RenderClear(renderer);
                SDL_Color hl = { 255, 200, 60, 255 };
                SDL_Color textColor = { 30, 30, 30, 255 };
                if (font) {
                    SDL_Surface* surf = TTF_RenderText_Blended(font, msg.c_str(), textColor);
                    if (surf) {
                        SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, surf);
                        int tw = surf->w, th = surf->h;
                        SDL_FreeSurface(surf);
                        SDL_Rect box = { WORLD_W/2 - (tw+40)/2, WORLD_H/2 - (th+30)/2, tw+40, th+30 };
                        SDL_SetRenderDrawColor(renderer, hl.r, hl.g, hl.b, hl.a);
                        SDL_RenderFillRect(renderer, &box);
                        if (tex) {
                            SDL_Rect dst = { box.x + 20, box.y + 12, tw, th };
                            SDL_RenderCopy(renderer, tex, NULL, &dst);
                            SDL_DestroyTexture(tex);
                        }
                    }
                }
                SDL_RenderPresent(renderer);
                SDL_Delay(16);
            }
        }
        SDL_DestroyTexture(green_texture);
        SDL_DestroyTexture(red_texture);
        SDL_DestroyTexture(wall_tex_h);
        SDL_DestroyTexture(wall_tex_v);
        // cleanup random walls created for PVE
        for (auto* rw : pve_random_walls) if (rw) delete rw;
        pve_random_walls.clear();
    // cleanup blackholes (PVE)
    for (auto& bhp : pve_blackholes_local) if (bhp.first) delete bhp.first;
    pve_blackholes_local.clear();
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
                        } else if (options[selected] == "PVE") {
                            run_pve_game();
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
                            } else if (options[selected] == "PVE") {
                                run_pve_game();
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

        // Title - prefer an image if present; remove the gray placeholder box
        if (title_tex) {
            int tw, th; SDL_QueryTexture(title_tex, NULL, NULL, &tw, &th);
            // scale title by 40% (30% + 10% requested)
            const float scale = 1.5f;
            int dw = (int)std::round(tw * scale);
            int dh = (int)std::round(th * scale);
            // clamp to window width with 40px margin
            if (dw > WINDOW_W - 40) {
                float s2 = float(WINDOW_W - 40) / float(tw);
                dw = WINDOW_W - 40;
                dh = (int)std::round(th * s2);
            }
            // move up by 20% of window height (but not negative)
            int titleY = 80 - (int)std::round(0.20f * WINDOW_H);
            if (titleY < 8) titleY = 8;
            SDL_Rect tdst = { WINDOW_W/2 - dw/2, titleY, dw, dh };
            SDL_RenderCopy(renderer, title_tex, NULL, &tdst);
        } else {
            // try to (re)load title texture from assets if the initial load failed (try png then jpg)
            if (!resourceManager.load_texture("title", "assets/pictures/title.png")) {
                resourceManager.load_texture("title", "assets/pictures/title.jpg");
            }
            title_tex = resourceManager.get_texture("title");
            if (title_tex) {
                int tw, th; SDL_QueryTexture(title_tex, NULL, NULL, &tw, &th);
                const float scale = 1.4f;
                int dw = (int)std::round(tw * scale);
                int dh = (int)std::round(th * scale);
                if (dw > WINDOW_W - 40) {
                    float s2 = float(WINDOW_W - 40) / float(tw);
                    dw = WINDOW_W - 40;
                    dh = (int)std::round(th * s2);
                }
                int titleY = 80 - (int)std::round(0.20f * WINDOW_H);
                if (titleY < 8) titleY = 8;
                SDL_Rect tdst = { WINDOW_W/2 - dw/2, titleY, dw, dh };
                SDL_RenderCopy(renderer, title_tex, NULL, &tdst);
            }
            // else: do not draw the gray placeholder box; simply omit the title
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

