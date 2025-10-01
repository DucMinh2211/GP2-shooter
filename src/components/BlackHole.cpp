#include "inc/BlackHole.h"
#include "inc/Character.h"
#include "inc/Bullet.h"
#include "inc/Circle.h"
#include <SDL2/SDL_render.h>
#include <iostream>

// Constructor
BlackHole::BlackHole(Vector2 pos, SDL_Texture* sprite, float outer_radius, float inner_radius,
                     float dps_outer, float dps_inner)
    : Obstacle(pos, sprite, {}), // Call base with empty hitbox list
      _outer_radius(outer_radius),
      _inner_radius(inner_radius),
      _dps_outer(dps_outer),
      _dps_inner(dps_inner) {
    
    // Create the two hitboxes automatically, using the BlackHole's world position as their center.
    _hitbox_list.push_back(new Circle(_position, _outer_radius));
    _hitbox_list.push_back(new Circle(_position, _inner_radius));
}

// Destructor
BlackHole::~BlackHole() {
    for (auto* hitbox : _hitbox_list) {
        delete hitbox;
    }
    _hitbox_list.clear();
}

void BlackHole::collide(ICollidable& object) {
    // Define constants for the effect strength.
    const float SUCK_IN_FORCE = 50.0f;
    const float BSUCK_IN_FORCE = 200.0f;
    const float OUTER_DAMAGE = 0.2f;
    const float INNER_DAMAGE = 0.8f;

    // --- Character Collision ---
    if (Character* character = dynamic_cast<Character*>(&object)) {
        bool inner_collision = false;
        bool outer_collision = false;

        HitBox* outer_hb = _hitbox_list[0];
        HitBox* inner_hb = _hitbox_list[1];

        for (auto* char_hb : character->get_hitboxes()) {
            if (inner_hb->is_collide(*char_hb)) {
                inner_collision = true;
                break;
            }
            if (outer_hb->is_collide(*char_hb)) {
                outer_collision = true;
            }
        }

        if (inner_collision) {
            character->take_damage(INNER_DAMAGE);
            Vector2 pull_direction = _position - character->get_position();
            if (pull_direction.length_squared() > 0) {
                character->add_force(pull_direction.normalize() * SUCK_IN_FORCE);
            }
        } else if (outer_collision) {
            character->take_damage(OUTER_DAMAGE);
            Vector2 pull_direction = _position - character->get_position();
            if (pull_direction.length_squared() > 0) {
                character->add_force(pull_direction.normalize() * SUCK_IN_FORCE);
            }
        }
        return; // Done with this object
    }

    // --- Bullet Collision ---
    if (Bullet* bullet = dynamic_cast<Bullet*>(&object)) {
        for (auto* bh_hb : this->get_hitboxes()) {
            for (auto* bullet_hb : bullet->get_hitboxes()) {
                if (bh_hb->is_collide(*bullet_hb)) {
                    // If a bullet hits any part of the black hole, suck it in.
                    Vector2 pull_direction = _position - bullet->get_position();
                    if (pull_direction.length_squared() > 0) {
                        bullet->add_force(pull_direction.normalize() * BSUCK_IN_FORCE);
                    }
                    return; // Apply force once per bullet per frame
                }
            }
        }
    }
}

// Logic has been moved to collide(), as requested.
void BlackHole::update(float delta_time) {
    if (_anim) _anim->update(delta_time);
}

// Render the black hole's sprite.
void BlackHole::render(SDL_Renderer* renderer) {
    if (_anim) {
        // AnimatedSprite render expects x,y center by our earlier change
        _anim->render(renderer, (int)_position.x - 213, (int)_position.y - 205, 2, 0.0);
        return;
    }

    if (!_sprite) return;

    int w, h;
    SDL_QueryTexture(_sprite, NULL, NULL, &w, &h);
    SDL_Rect dst_rect = {
        (int)(_position.x - w / 2.0f),
        (int)(_position.y - h / 2.0f),
        w,
        h
    };
    SDL_RenderCopy(renderer, _sprite, NULL, &dst_rect);
}

