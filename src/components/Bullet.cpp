#include "Constant.h"
#include "inc/Bullet.h"
#include "inc/Rect.h"
#include "inc/Circle.h"
#include "SDL2/SDL_render.h"
#include "inc/OBB.h"
#include <iostream>

Bullet::Bullet(Vector2 position, SDL_Texture* sprite, float speed, float damage, Vector2 init_direction, BulletBuffType buffed)
    : Entity(position, sprite, BULLET_SPEED), _damage(damage), _init_direction(init_direction), _buffed(buffed) {}

void Bullet::add_hitbox(HitBox* hitbox) {
    _hitbox_list.push_back(hitbox);
}

void Bullet::update_hitboxes() {
    float angle = std::atan2(_init_direction.y, _init_direction.x);

    for (auto* hitbox : _hitbox_list) {
        if (auto* obb = dynamic_cast<OBB*>(hitbox)) {
            // center căn giữa sprite 24x24
            Vector2 center = _position + Vector2(24.0f / 2.0f, 24.0f / 2.0f);
            obb->set_transform(center, angle);
        }
    }
}


void Bullet::render(SDL_Renderer* renderer) {
    SDL_Rect srcRect = {4, 0, 20, 24}; // bullet sprite in sheet
    SDL_Rect bullet_rect = { (int)this->_position.x, (int)this->_position.y, 24, 24 };
    float angle = std::atan2(this->_init_direction.y, this->_init_direction.x) * 180.0f / PI;
    SDL_RenderCopyEx(renderer, this->_sprite, &srcRect, &bullet_rect, angle, NULL, SDL_FLIP_NONE);


    //Debug hibox | comment sau khi debug xong
    SDL_Color debugColor = {255, 0, 0, 255};
    for (auto* hitbox : _hitbox_list) {
        hitbox->debug_draw(renderer, debugColor);
    }
}

void Bullet::update(float delta_time) {
    // Move bullet
    _position += _init_direction * _speed * delta_time;
    update_hitboxes();
}

void Bullet::collide(ICollidable& object) {
    // Lặp qua tất cả hitbox của bullet
    for (auto* hb1 : this->_hitbox_list) {
        if (auto* obb1 = dynamic_cast<OBB*>(hb1)) {
            
            // Lặp qua tất cả hitbox của object
            for (auto* hb2 : object.get_hitboxes()) {
                if (auto* obb2 = dynamic_cast<OBB*>(hb2)) {
                    // Kiểm tra collision OBB vs OBB
                    if (obb1->is_collide(*obb2)) {
                        // Va chạm xảy ra, xử lý logic
                        std::cout << "Bullet hit an object!" << std::endl;
                    }
                }
            }
        }
    }
}

