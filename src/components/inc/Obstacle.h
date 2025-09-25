#pragma once

#include "ICollidable.h"
#include <vector>

// Forward declarations
class HitBox;
struct Vector2;
class Texture;

class Obstacle : public ICollidable {
public:
    virtual ~Obstacle() = default;
    Vector2 get_position();
    Texture* get_sprite();
    void collide(std::vector<HitBox> hitbox_list) override = 0;

protected:
    std::vector<HitBox*> _hitbox_list;
    Vector2* _position;
    Texture* _sprite;
};
