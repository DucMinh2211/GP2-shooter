#pragma once

#include "Entity.h"

// Forward declarations
struct Vector2;
class Rect;
class Texture;

// Placeholder for Enum
enum class BulletBuffType {};

class Bullet : public Entity {
public:
    Vector2 get_position();
    Texture* get_sprite();
    Rect* get_collision();
    void update(float delta_time) override;
    void collide(Rect& object) override;

private:
    float _damage;
    Vector2* _direction;
    BulletBuffType _buffed;
};
