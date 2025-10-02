#include "Constant.h"
#include "inc/Bullet.h"
#include "inc/Explosion.h"
#include "inc/Rect.h"
#include "inc/Circle.h"
#include "SDL2/SDL_render.h"
#include "inc/OBB.h"
#include "inc/Wall.h"
#include <iostream>
#include <vector>

Bullet::Bullet(Vector2 position, SDL_Texture* sprite, float speed, float damage, Vector2 init_direction, BulletBuffType buffed, int team_id)
    : Entity(position, sprite, BULLET_SPEED), _team_id(team_id), _damage(damage), _init_direction(init_direction), _buffed(buffed), _is_destroyed(false) {}

Bullet::~Bullet() {
    for (auto* hitbox : _hitbox_list) {
        delete hitbox;
    }
    _hitbox_list.clear();
}

void Bullet::add_hitbox(HitBox* hitbox) {
    _hitbox_list.push_back(hitbox);
}

void Bullet::update_hitboxes() {
    float angle = std::atan2(_init_direction.y, _init_direction.x);

    for (auto* hitbox : _hitbox_list) {
        if (auto* obb = dynamic_cast<OBB*>(hitbox)) {
            // center căn giữa sprite 24x24
            Vector2 center = _position; 
            obb->set_transform(center, angle);
        }
    }
}

void Bullet::explode(std::vector<Explosion*>& explosions, SDL_Renderer* renderer) {
    explosions.push_back(new Explosion(renderer, EXPLOSION_TEXTURE_PATH, this->_position, 50, 50, 9 ,40, 3, 25.0f));
}

void Bullet::render(SDL_Renderer* renderer) {
    SDL_Rect srcRect = {4, 0, 20, 24}; // bullet sprite in sheet
    int w = 24, h = 24;
    SDL_Rect bullet_rect = { (int)this->_position.x - w/2, (int)this->_position.y - h/2, w, h };
    float angle = std::atan2(this->_init_direction.y, this->_init_direction.x) * 180.0f / PI;
    if (this->_sprite) {
        SDL_RenderCopyEx(renderer, this->_sprite, &srcRect, &bullet_rect, angle, NULL, SDL_FLIP_NONE);
    }


    //Debug hibox | comment sau khi debug xong
    // SDL_Color debugColor = {255, 0, 0, 255};
    // for (auto* hitbox : _hitbox_list) {
    //     hitbox->debug_draw(renderer, debugColor);
    // }
}

void Bullet::update(float delta_time) {
    // Calculate velocity and apply force
    Vector2 initial_velocity = _init_direction * _speed;
    Vector2 final_velocity = initial_velocity + _force;
    _position += final_velocity * delta_time;
    
    update_hitboxes();

    // Reset force for the next frame
    _force = ZERO;
}

void Bullet::add_force(Vector2 force) {
    _force += force;
}

void Bullet::collide(ICollidable* object) {
    if (!object) return;

    // Iterate bullet hitboxes (expecting OBB)
    for (auto* hb1 : this->_hitbox_list) {
        auto* obb1 = dynamic_cast<OBB*>(hb1);
        if (!obb1) continue;

        for (auto* hb2 : object->get_hitboxes()) {
            auto* obb2 = dynamic_cast<OBB*>(hb2);
            if (!obb2) continue;

            if (obb1->is_collide(*obb2)) {
                // If object is a wall
                if (typeid(*object) == typeid(Wall)) {
                    if (isPiercing()) {
                        // Do nothing (passes through)
                        return; // no further processing for this wall
                    } else if (isBouncing()) {
                        // Reflect initial direction around collision normal approximation
                        // Approximate normal: pick axis with greater penetration direction
                        Vector2 diff = _position - ((OBB*)obb2)->get_center();
                        if (std::abs(diff.x) > std::abs(diff.y)) {
                            _init_direction.x = -_init_direction.x; // reflect horizontally
                        } else {
                            _init_direction.y = - _init_direction.y; // reflect vertically
                        }
                        // Normalize and slightly reduce speed to avoid infinite loops (optional)
                        _init_direction.normalize();
                        // Update hitbox orientation after bounce
                        update_hitboxes();
                        return; // bounce handled
                    } else {
                        // Normal bullet: destroy
                        _is_destroyed = true;
                        return;
                    }
                } else {
                    // Other objects: keep old behavior (destroy)
                    _is_destroyed = true;
                    return;
                }
            }
        }
    }
}

