#include "inc/Character.h"
#include "ResourceManager.h"
#include "inc/Bullet.h"
#include "inc/CharBuff.h"
#include "inc/Explosion.h"
#include "inc/Wall.h"
#include "inc/BuffItem.h"
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
    // Dead logic: no movement, no shooting, play dead animation
    if (_health <= 0.0f) {
        _direction = Vector2{0,0};
        _force = Vector2{0,0};
        if (_dead_anim) _current_anim = _dead_anim; else _current_anim = _idle_anim;
        if (_current_anim) _current_anim->update(delta_time);
        return;
    }

    if (_direction.length_squared() > 0) {
        _direction.normalize();
        _last_direction = _direction;
        _angle = std::atan2(_direction.y, _direction.x);
    }

    Vector2 velocity = _direction * _speed + _force;

    // store last movement delta so collisions with walls can undo it
    _last_move_vec = velocity * delta_time;
    _position += _last_move_vec;
    for (auto* hb : _hitbox_list) {
        if (auto* obb = dynamic_cast<OBB*>(hb)) {
            obb->set_transform(_position, _angle); // cập nhật center và góc
        }
    }

    // update _shoot_delay
    _shoot_delay -= delta_time;
    std::max(_shoot_delay, 0.0f);

    // update CharBuffList
    for (CharBuff char_buff : _buff_list) {
        char_buff.update(delta_time);
    }

    // update _gun_buffed
    _gun_buffed.update(delta_time);

    // cập nhật shoot timer
    if (_shoot_timer > 0.0f) {
        _shoot_timer -= delta_time;
        _current_anim = _shoot_anim;
    } else if (_direction.length_squared() == 0) {
        _current_anim = _idle_anim;
    } else {
        _current_anim = _run_anim;
    }

    if (_current_anim)
        _current_anim->update(delta_time);
    _force = Vector2{0,0};

    // Keep character inside world bounds (use character hitbox half-size of 8)
    const float halfSize = 8.0f;
    if (_position.x < halfSize) _position.x = halfSize;
    if (_position.y < halfSize) _position.y = halfSize;
    if (_position.x > WORLD_W - halfSize) _position.x = WORLD_W - halfSize;
    if (_position.y > WORLD_H - halfSize) _position.y = WORLD_H - halfSize;
    _force = Vector2{0,0};
    // reset lực hoặc flag nếu muốn
}


void Character::set_direction(Vector2 direction) {
    _direction = direction;
    if (_direction.length_squared() > 0) {
        _last_direction = _direction; // chỉ update khi có hướng di chuyển
        _angle = std::atan2(_direction.y, _direction.x) * 180.0f / PI;
    }
}

void Character::take_damage(float amount) {
    this->_health -= amount/10;
    this->_health = std::max(this->_health, 0.0f);
}

void Character::add_force(Vector2 force) {
    this->_force += force;
}

void Character::collide(ICollidable* object) {
    if (typeid(*object) == typeid(Bullet)) {
        Bullet* bullet = dynamic_cast<Bullet*>(object);
        if (!bullet || bullet->get_team_id() == this->_input_set) {
            return; // Not a bullet or friendly fire
        }

        for (auto* char_hb : this->get_hitboxes()) {
            for (auto* bullet_hb : object->get_hitboxes()) {
                if (char_hb->is_collide(*bullet_hb)) {
                    // Collision detected, apply damage and exit
                    this->_health -= bullet->get_damage();
                    if (this->_health <= 0.0f) {
                        std::cout << "player dead\n";
                    }
                    this->_health = std::max(this->_health, 0.0f);
                    bullet->set_destroyed(true);
                    return;
                }
            }
        }
    }
    else if (typeid(*object) == typeid(Wall)) {
        Wall* wall = dynamic_cast<Wall*>(object);
        if (!wall) return;

        for (auto* char_hb : this->get_hitboxes()) {
            for (auto* wall_hb : object->get_hitboxes()) {
                if (char_hb->is_collide(*wall_hb)) {
                    // Collision with wall detected, undo last movement.
                    this->_position -= this->_last_move_vec;
                    update_character_hitboxes(this); // Re-update hitboxes to new position
                    return;
                }
            }
        }
    }
    else if (typeid(*object) == typeid(BuffItem)) {
        BuffItem* buff_item = dynamic_cast<BuffItem*>(object);
        if (!buff_item) return;

        for (auto* char_hb : this->get_hitboxes()) {
            for (auto* buff_hb : object->get_hitboxes()) {
                if (char_hb->is_collide(*buff_hb)) {
                    std::variant<CharBuffType, BulletBuffType> buff_type = buff_item->get_buff_type();
                    if (std::holds_alternative<BulletBuffType>(buff_type)) {
                        this->_gun_buffed.set_type(std::get<BulletBuffType>(buff_type));
                        buff_item->consume();
                    } else if (std::holds_alternative<CharBuffType>(buff_type)) {
                        CharBuffType char_buff_type = std::get<CharBuffType>(buff_type);
                        if (char_buff_type == CharBuffType::HEALTH) {
                            this->_health += 10;
                            buff_item->consume();
                        } else if (char_buff_type == CharBuffType::SPEED) {
                            this->_speed += 100.0f;
                            buff_item->consume();
                            this->_buff_list.at((size_t)char_buff_type).timer_start();
                        }
                    }
                    return;
                }
            }
        }
    }
    else if (Explosion *explosion = dynamic_cast<Explosion*>(object)) {
        for (auto* char_hb : this->get_hitboxes()) {
            for (auto* e_hb : object->get_hitboxes()) {
                if (char_hb->is_collide(*e_hb)) {
                    if (explosion->damageable) {
                        std::cout << "DMG!";
                        this->_health -= explosion->get_damage();
                        explosion->damageable = false;
                    }
                    return;
                }
            }
        }
    }
}

void Character::remove_buff(CharBuffType buff_type) {
    // TODO: implement
    switch (buff_type) {
        case CharBuffType::SPEED: {
            this->_speed -= 100.0f;
            break;
        }
        default: return;
    }
}

void Character::shoot(std::vector<Bullet*>& bullet_list, ResourceManager& resource_manager) {
    if (_health <= 0.0f) return; // dead can't shoot
    if (this->_shoot_delay > 0) return;


    SDL_Texture* bullet_sprite = resource_manager.get_texture("bullet");
    // Create a bullet at the character's position, facing _direction
    Vector2 bullet_dir = (_direction.length_squared() > 0) ? _direction : _last_direction;
    Vector2 bullet_pos = {_position.x,_position.y };
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
    _shoot_timer = _shoot_duration; 
}

void Character::render(SDL_Renderer* renderer) {
    // TODO: implement
    if (_current_anim) {
    Vector2 pos = get_position();
    double angle_deg = _angle * 180.0 / M_PI;
    _current_anim->render(renderer, (int)pos.x - 12, (int)pos.y - 8, 1,angle_deg);
    }

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
