#pragma once

#include "ICollidable.h"
#include "IUpdatable.h"
#include "math/Vector2.h"
#include "HitBox.h"
#include <vector>

// Forward Declaration
struct SDL_Texture;

class Entity : public ICollidable, public IUpdatable {
protected:
    Vector2 _position;
    SDL_Texture* _sprite;
    float _speed;
    Vector2 _force = ZERO;
    std::vector<HitBox*> _hitbox_list;

public:
    Entity(Vector2 position, SDL_Texture* sprite, float speed) : _position(position), _sprite(sprite), _speed(speed) {}
    Vector2 get_position() const { return this->_position; }
    const SDL_Texture* get_sprite() const { return this->_sprite; }
    const std::vector<HitBox*> get_collision() const {
        return this->_hitbox_list;
    }
    virtual void collide(ICollidable& object) override = 0;
    virtual void update(float delta_time) override = 0;
};
