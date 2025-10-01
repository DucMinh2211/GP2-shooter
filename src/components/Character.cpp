#include "inc/Character.h"
#include "ResourceManager.h"
#include "inc/Bullet.h"
#include "inc/CharBuff.h"
#include "inc/Wall.h"
#include "math/Vector2.h"
#include "SDL2/SDL_render.h"
#include <SDL2/SDL_error.h>
#include <algorithm>
#include <iostream>
#include <vector>
#include <typeinfo>
#include "Constant.h"
#include "inc/OBB.h"

Character::Character(Vector2 position, SDL_Texture* sprite, float speed, float health) : Entity(position, sprite, speed), _health(health) {
    // init buff_list
    for (int buff_type = 0; buff_type < (int)CharBuffType::NUM; buff_type++) {
        CharBuff buff =  CharBuff(CharBuff::DURATION_LIST[buff_type], (CharBuffType)buff_type, *this);
        this->_buff_list.push_back(buff);
    }
    this->_shoot_delay = SHOOT_DELAY_MAP.at(this->_gun_type);

    // Create a 16x16 OBB hitbox for the character
    OBB* characterHitbox = new OBB(this->_position, Vector2(8.0f, 8.0f), 0.0f);
    this->_hitbox_list.push_back(characterHitbox);
}

void Character::set_activate(bool activated) {
    this->_activated = activated;
}

void Character::set_input_set(int input_set) {
    this->_input_set = input_set;
}

// Private helper to update hitbox positions
void update_character_hitboxes(Character* character) {
    for (auto* hitbox : character->get_hitboxes()) {
        if (auto* obb = dynamic_cast<OBB*>(hitbox)) {
            // The character's position is its center, which is also the OBB's center.
            obb->set_transform(character->get_position(), 0.0f);
        }
    }
}

void Character::update(float delta_time) {
    // Normalize the direction vector if it's not zero
    if (this->_direction.length_squared() > 0) {
        this->_direction.normalize();
    }

    // Calculate velocity from input and speed
    Vector2 input_velocity = this->_direction * this->_speed;

    // Combine with external forces from the previous frame's collisions
    Vector2 final_velocity = input_velocity + this->_force;

    // Store movement vector and update position
    this->_last_move_vec = final_velocity * delta_time;
    this->_position += this->_last_move_vec;

    // Update hitboxes to follow the character
    update_character_hitboxes(this);

    // Reset force for the next frame
    _force = ZERO;
}

void Character::set_direction(Vector2 direction) {
    _direction = direction;
    if (_direction.length_squared() > 0) {
        _last_direction = _direction; // chỉ update khi có hướng di chuyển
    }
}

void Character::take_damage(float amount) {
    this->_health -= amount;
}

void Character::add_force(Vector2 force) {
    this->_force += force;
}

void Character::collide(ICollidable& object) {
    if (typeid(object) == typeid(Bullet)) {
        Bullet* bullet = dynamic_cast<Bullet*>(&object);
        if (!bullet || bullet->get_team_id() == this->_input_set) {
            return; // Not a bullet or friendly fire
        }

        for (auto* char_hb : this->get_hitboxes()) {
            for (auto* bullet_hb : object.get_hitboxes()) {
                if (char_hb->is_collide(*bullet_hb)) {
                    // Collision detected, apply damage and exit
                    this->_health -= bullet->get_damage();
                    if (this->_health <= 0) {
                        // std::cout << "player dead\n";
                    }
                    return;
                }
            }
        }
    }
    else if (typeid(object) == typeid(Wall)) {
        Wall* wall = dynamic_cast<Wall*>(&object);
        if (!wall) return;

        for (auto* char_hb : this->get_hitboxes()) {
            for (auto* wall_hb : object.get_hitboxes()) {
                if (char_hb->is_collide(*wall_hb)) {
                    // Collision with wall detected, undo last movement.
                    this->_position -= this->_last_move_vec;
                    update_character_hitboxes(this); // Re-update hitboxes to new position
                    return;
                }
            }
        }
    }
}

void Character::remove_buff(CharBuffType buff_type) {
    // TODO: implement
}

void Character::shoot(std::vector<Bullet*>& bullet_list, ResourceManager& resource_manager) {
    SDL_Texture* bullet_sprite = resource_manager.get_texture("bullet");
    // Create a bullet at the character's position, facing _direction
    Vector2 bullet_dir = (_direction.length_squared() > 0) ? _direction : _last_direction;
    Vector2 bullet_pos = {_position.x - 10,_position.y - 10};
    std::cout << "input_set when shoot " << this->_input_set << "\n";
    Bullet* bullet = new Bullet(bullet_pos, bullet_sprite, BULLET_SPEED, 10.0f, bullet_dir, _gun_buffed.getType(), this->_input_set);
    int hb_width = 15;
    int hb_height = 8;

    // Tính tâm (center) của hitbox (nằm giữa sprite 24x24)
    float hb_x = _position.x + 24.0f / 2.0f;
    float hb_y = _position.y + 24.0f / 2.0f;

    // halfSize = (width/2, height/2)
    Vector2 halfSize(hb_width / 2.0f, hb_height / 2.0f);

    // Góc ban đầu theo hướng bay của đạn
    float angle = std::atan2(bullet_dir.y, bullet_dir.x);

    // Tạo OBB hitbox
    OBB* bulletHitbox = new OBB(Vector2(hb_x, hb_y), halfSize, angle);
    bullet->add_hitbox(bulletHitbox);

    // Push bullet vào danh sách
    bullet_list.push_back(bullet);
}

void Character::render(SDL_Renderer* renderer) {
    // TODO: implement

    if (this->_activated) this->render_activated_circle(renderer);
}

void Character::render_activated_circle(SDL_Renderer *renderer) {
    if (_input_set == 0) {
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); // Red
    } else {
        SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255); // Blue
    }

    int center_x = _position.x;
    int center_y = _position.y;

   int w, h;

    if (SDL_QueryTexture(this->_sprite, nullptr, nullptr, &w, &h)) {
        std::cerr << "Fail to get sprite texture size: " << SDL_GetError();
    }
    const int sprite_radius = std::max(w, h);

    for (int r_offset = 0; r_offset < 2; ++r_offset) {
        const int radius = sprite_radius + r_offset;
        const int diameter = (radius * 2);

        int x = (radius - 1);
        int y = 0;
        int tx = 1;
        int ty = 1;
        int error = (tx - diameter);

        while (x >= y) {
            // Each of the following renders an octant of the circle
            SDL_RenderDrawPoint(renderer, center_x + x, center_y - y);
            SDL_RenderDrawPoint(renderer, center_x + x, center_y + y);
            SDL_RenderDrawPoint(renderer, center_x - x, center_y - y);
            SDL_RenderDrawPoint(renderer, center_x - x, center_y + y);
            SDL_RenderDrawPoint(renderer, center_x + y, center_y - x);
            SDL_RenderDrawPoint(renderer, center_x + y, center_y + x);
            SDL_RenderDrawPoint(renderer, center_x - y, center_y - x);
            SDL_RenderDrawPoint(renderer, center_x - y, center_y + x);

            if (error <= 0) {
                y++;
                error += ty;
                ty += 2;
            }

            if (error > 0) {
                x--;
                tx += 2;
                error += (tx - diameter);
            }
        }
    }
}
