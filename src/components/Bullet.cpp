#include "Constant.h"
#include "inc/Bullet.h"
#include "inc/Rect.h"
#include "inc/Circle.h"
#include "SDL2/SDL_render.h"

Bullet::Bullet(Vector2 position, SDL_Texture* sprite, float speed, float damage, Vector2 init_direction, BulletBuffType buffed)
    : Entity(position, sprite, speed), _damage(damage), _init_direction(init_direction), _buffed(buffed) {}

void Bullet::add_hitbox(HitBox* hitbox) {
    _hitbox_list.push_back(hitbox);
}

void Bullet::update_hitboxes() {
    // Update all hitboxes to match bullet's position
    for (auto* hitbox : _hitbox_list) {
        // For Rect
        if (auto* rect = dynamic_cast<Rect*>(hitbox)) {
            SDL_Rect r = rect->get_rect();
            r.x = static_cast<int>(_position.x);
            r.y = static_cast<int>(_position.y);
            // You may want to add a set_rect method to Rect to update its SDL_Rect
            // rect->set_rect(r);
        }
        // For Circle
        else if (auto* circle = dynamic_cast<Circle*>(hitbox)) {
            // You may want to add a set_local_pos method to HitBox/Circle
            // circle->set_local_pos(_position);
        }
    }
}

void Bullet::render(SDL_Renderer* renderer) {
    SDL_Rect srcRect = {0, 0, 24, 24}; // bullet sprite in sheet
    SDL_Rect bullet_rect = { (int)this->_position.x, (int)this->_position.y, 24, 24 };
    float angle = std::atan2(this->_init_direction.x, this->_init_direction.y) * 180.0f / PI;
    SDL_RenderCopyEx(renderer, this->_sprite, &srcRect, &bullet_rect, angle, NULL, SDL_FLIP_NONE);
}

void Bullet::update(float delta_time) {
    // Move bullet
    _position += _init_direction * _speed * delta_time;
    update_hitboxes();
}

void Bullet::collide(ICollidable& object) {
    // Implement collision response here
}
