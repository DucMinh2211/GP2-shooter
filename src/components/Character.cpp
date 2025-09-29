#include "inc/Character.h"
#include "ResourceManager.h"
#include "inc/Entity.h"
#include "inc/Bullet.h"
#include "inc/CharBuff.h"
#include "inc/Wall.h"
#include "math/Vector2.h"
#include "SDL2/SDL_render.h"
#include <vector>
#include <typeinfo>
#include "Constant.h"
#include "inc/OBB.h"

Character::Character(Vector2 position, SDL_Texture* sprite, float speed, float health, bool activate) : Entity(position, sprite, speed), _health(health) {
    // init buff_list
    for (int buff_type = 0; buff_type < (int)CharBuffType::NUM; buff_type++) {
        CharBuff buff =  CharBuff(CharBuff::DURATION_LIST[buff_type], (CharBuffType)buff_type, *this);
        this->_buff_list.push_back(buff);
    }
}

void Character::update(float delta_time) {
    // Normalize the direction vector if it's not zero
    if (this->_direction.length_squared() > 0) {
        this->_direction.normalize();
    }

    // Calculate velocity from input and speed
    Vector2 input_velocity = this->_direction * this->_speed;

    // Combine with external forces
    Vector2 final_velocity = input_velocity + this->_force;

    // Update position
    this->_position += final_velocity * delta_time;
}

void Character::set_direction(Vector2 direction) {
    _direction = direction;
    if (_direction.length_squared() > 0) {
        _last_direction = _direction; // chỉ update khi có hướng di chuyển
    }
}

void Character::collide(ICollidable& object) {
    if (typeid(object) == typeid(Bullet)) {
        Bullet* bullet = dynamic_cast<Bullet*>(&object);
        if (bullet) {
            this->_health -= bullet->get_damage();
        }
    }// else if (typeid(object) == typeid(Wall)) {
    //     Wall* wall = dynamic_cast<Wall*>(&object);
    // }
}

void Character::remove_buff(CharBuffType buff_type) {
    // TODO: implement
}

void Character::shoot(std::vector<Bullet*>& bullet_list, ResourceManager& resource_manager) {
    SDL_Texture* bullet_sprite = resource_manager.get_texture("bullet");
    // Create a bullet at the character's position, facing _direction
    Vector2 bullet_dir = (_direction.length_squared() > 0) ? _direction : _last_direction;
    Vector2 bullet_pos = {_position.x,_position.y - 10};
    Bullet* bullet = new Bullet(bullet_pos, bullet_sprite, BULLET_SPEED, 10.0f, bullet_dir, _gun_buffed.getType());
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
}
