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
    explosions.push_back(new Explosion(renderer, EXPLOSION_TEXTURE_PATH, this->_position, 50, 50, 9 ,40, 3, 25.0f, this->_team_id));
}

void Bullet::render(SDL_Renderer* renderer) {
    SDL_Rect srcRect = {4, 0, 20, 24}; // bullet sprite in sheet
    int w = 24, h = 24;
    SDL_Rect bullet_rect = { (int)this->_position.x - w/2, (int)this->_position.y - h/2, w, h };
    float angle = std::atan2(this->_init_direction.y, this->_init_direction.x) * 180.0f / PI;
    SDL_RenderCopyEx(renderer, this->_sprite, &srcRect, &bullet_rect, angle, NULL, SDL_FLIP_NONE);


    //Debug hibox | comment sau khi debug xong
    // SDL_Color debugColor = {255, 0, 0, 255};
    // for (auto* hitbox : _hitbox_list) {
    //     hitbox->debug_draw(renderer, debugColor);
    // }
}

void Bullet::update(float delta_time) {
    // calc _life_timer
    this->_life_timer -= delta_time;
    if (this->_life_timer <= 0) this->_is_destroyed = true;

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
                        // Proper reflection using OBB closest point normal
                        OBB* other = static_cast<OBB*>(obb2);
                        Vector2 C = other->get_center();
                        Vector2 half = other->get_halfSize();
                        float a = other->get_angle();

                        // Helpers: rotate point by angle
                        auto rotate = [&](const Vector2& p, float ang) {
                            float ca = std::cos(ang);
                            float sa = std::sin(ang);
                            return Vector2(p.x * ca - p.y * sa, p.x * sa + p.y * ca);
                        };

                        // transform point into OBB local space
                        Vector2 local = rotate(_position - C, -a);

                        // clamp to box extents
                        Vector2 clamped = local;
                        if (clamped.x > half.x) clamped.x = half.x;
                        if (clamped.x < -half.x) clamped.x = -half.x;
                        if (clamped.y > half.y) clamped.y = half.y;
                        if (clamped.y < -half.y) clamped.y = -half.y;

                        // closest point in world space
                        Vector2 closest = rotate(clamped, a) + C;

                        Vector2 normal = _position - closest;
                        float nlen = normal.length();
                        if (nlen == 0.0f) {
                            // Degenerate: fallback to direction from box center
                            normal = (_position - C);
                            nlen = normal.length();
                            if (nlen == 0.0f) {
                                // give an arbitrary normal
                                normal = Vector2(0.0f, -1.0f);
                                nlen = 1.0f;
                            }
                        }
                        normal /= nlen; // normalize

                        // incoming velocity (including transient force)
                        Vector2 incoming = _init_direction * _speed + _force;
                        // reflect: r = v - 2*(v·n)*n
                        float dotvn = Vector2::dot(incoming, normal);
                        Vector2 reflected = incoming - normal * (2.0f * dotvn);

                        // set new direction from reflected vector (only direction matters)
                        if (reflected.length_squared() > 0.0f) {
                            reflected.normalize();
                            _init_direction = reflected;
                        } else {
                            // fallback: flip one axis
                            _init_direction.x = -_init_direction.x;
                            _init_direction.y = -_init_direction.y;
                            _init_direction.normalize();
                        }

                        // reset transient forces so next frame uses only the new direction
                        _force = ZERO;

                        // Nudge bullet out along normal a small amount to avoid re-penetration
                        const float nudge = 1.5f;
                        _position += normal * nudge;

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

