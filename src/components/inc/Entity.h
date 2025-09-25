#pragma once

#include "ICollidable.h"
#include "IUpdatable.h"
#include "math/Vector2.h"
#include <vector>

// Forward declarations
class HitBox;
class Texture;

class Entity : public ICollidable, public IUpdatable {
public:
    Vector2 get_position();
    Texture* get_sprite();
    void collide(std::vector<HitBox> hitbox_list) override;
    void update(float delta_time) override;

protected:
    Vector2* _position;
    Texture* _sprite;
    float _speed;
    Vector2* _force;
    std::vector<HitBox*> _hitbox_list;
};
