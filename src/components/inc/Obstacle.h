#pragma once

#include "ICollidable.h"
#include <vector>

// Forward declarations
class HitBox;
struct Vector2;
class Texture;

class Obstacle : public ICollidable {
protected:
    std::vector<HitBox*> _hitbox_list;
    Vector2* _position;
    Texture* _sprite;

public:
    virtual ~Obstacle() = default;
    Vector2 get_position();
    Texture* get_sprite();
    std::vector<HitBox*>& get_hitboxes() override {
    return _hitbox_list;
}

    virtual void collide(ICollidable& object) override = 0;
};
