#include "inc/Character.h"
#include "ResourceManager.h"
#include "inc/Entity.h"
#include "inc/Bullet.h"
#include "inc/CharBuff.h"
#include "inc/Wall.h"
#include "math/Vector2.h"
#include "SDL2/SDL_render.h"
#include <vector>

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
    this->_direction = direction;
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
    Bullet* bullet = new Bullet(_position, bullet_sprite, _speed, 10.0f, _direction, _gun_buffed.getType());
    // Add a rectangular hitbox (8x8) at the bullet's position
    bullet->add_hitbox(new Rect(_position, SDL_Rect{(int)_position.x, (int)_position.y, 8, 8}));
    bullet_list.push_back(bullet);
}

void Character::render(SDL_Renderer* renderer) {
    // TODO: implement
}
