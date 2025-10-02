#include "inc/Explosion.h"
#include "inc/Character.h"
#include "inc/Bullet.h"
#include "inc/Circle.h"
#include "inc/Obstacle.h"
#include <SDL2/SDL.h>
#include <unordered_set>

Explosion::Explosion(SDL_Renderer* renderer, const std::string& sheetPath, Vector2 pos,
                     int frameW, int frameH, int frameCount, int frameTime, int columns, float damage, int owner_team)
    : Obstacle(pos, nullptr, {}), elapsed(0.0f), finished(false), _damage(damage), _owner_team(owner_team) {
    anim = new AnimatedSprite(renderer, sheetPath, frameW, frameH, frameCount, frameTime, columns);
    totalDurationMs = frameCount * frameTime;
    // Add a circular hitbox with radius equal to half the frame size
    Circle* hb = new Circle(_position, std::min(frameW, frameH) / 2.0f);
    _hitbox_list.push_back(hb);
}

Explosion::~Explosion() {
    delete anim;
    for (auto* hb : _hitbox_list) delete hb;
    _hitbox_list.clear();
}

void Explosion::update(float dt) {
    if (finished) return;
    elapsed += dt * 1000.0f;
    anim->update(dt);
    if (elapsed >= totalDurationMs) finished = true;
}

void Explosion::render(SDL_Renderer* renderer) {
    if (finished) return;
    anim->render(renderer, (int)_position.x - 30, (int)_position.y - 26, 1, 0.0);
}

void Explosion::collide(ICollidable* object) {
    // If object is a Character, apply damage once per-character
    if (Character* ch = dynamic_cast<Character*>(object)) {
        // if we've already damaged this character, skip
        if (_damaged.find(ch) == _damaged.end()) {
            for (auto* ch_hb : ch->get_hitboxes()) {
                for (auto* ex_hb : _hitbox_list) {
                    if (ex_hb->is_collide(*ch_hb)) {
                        float dmg = _damage;
                        ch->take_damage(dmg);
                        _damaged.insert(ch);
                        // only damage once per character
                        return;
                    }
                }
            }
        }
    }

    // If object is a Bullet, destroy or apply force
    if (Bullet* b = dynamic_cast<Bullet*>(object)) {
        for (auto* b_hb : b->get_hitboxes()) {
            for (auto* ex_hb : _hitbox_list) {
                if (ex_hb->is_collide(*b_hb)) {
                    // apply damage (we'll remove bullet externally)
                    b->set_destroyed(true);
                }
            }
        }
    }
}
