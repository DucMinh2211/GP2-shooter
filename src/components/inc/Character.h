#pragma once

#include "Entity.h"
#include "BulletBuff.h"
#include "CharBuff.h"

enum class GunType {
    NONE = 0,
    PISTOL = 1,
    AK = 2,
    SHOTGUN = 3,
    NUM = 4, // size of guntype
};

class Character : public Entity {
private:
    float _health;
    bool _activated;
    GunType _gun_type = GunType::NONE;
    SDL_Texture* _gun_sprite;
    BulletBuff _gun_buffed = BulletBuff(INFINITY, BulletBuffType::NONE);
    std::vector<CharBuff> _buff_list;

public:
    Character(Vector2 position, SDL_Texture* sprite, float speed, float health, bool activate);
    HitBox* get_collision();
    void shoot();
    void move(float dx, float dy);
    void update(float delta_time) override;
    void collide(ICollidable& object) override;
    void remove_buff(CharBuff& buff, CharBuffType buff_type);
};
