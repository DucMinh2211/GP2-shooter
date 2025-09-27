#pragma once

#include "Entity.h"
#include "ICollidable.h"

// Placeholder for Enum
enum class BulletBuffType {};

class Bullet : public Entity {
public:
    void update(float delta_time) override;
    void collide(ICollidable object) override;

private:
    float _damage;
    Vector2* _direction;
    BulletBuffType _buffed;
};
