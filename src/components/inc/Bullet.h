#pragma once

#include "Entity.h"
#include "HitBox.h"

// Placeholder for Enum
enum class BulletBuffType {};

class Bullet : public Entity {
public:
    Vector2 get_position();
    SDL_Texture* get_sprite();
    HitBox get_collision();
    void update(float delta_time) override;
    void collide(std::vector<HitBox> object) override;

private:
    float _damage;
    Vector2* _direction;
    BulletBuffType _buffed;
};
