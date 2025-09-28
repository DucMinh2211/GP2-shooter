#pragma once

#include "Entity.h"
#include "ICollidable.h"
#include "BulletBuff.h"

class Bullet : public Entity {
public:
    Bullet(Vector2 position, SDL_Texture* sprite, float speed, float damage, Vector2 init_direction, BulletBuffType buffed) : Entity(position, sprite, speed), _damage(damage), _init_direction(init_direction), _buffed(buffed) {}
    void update(float delta_time) override;
    void collide(ICollidable& object) override;

private:
    float _damage;
    Vector2 _init_direction;
    BulletBuffType _buffed;
};
