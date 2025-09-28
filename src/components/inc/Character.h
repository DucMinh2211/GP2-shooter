#pragma once

#include "Entity.h"
#include "BulletBuff.h"
#include "CharBuff.h"
#include "IInputObject.h"

enum class GunType {
    NONE = 0,
    PISTOL = 1,
    AK = 2,
    SHOTGUN = 3,
    NUM = 4, // size of guntype
};

class Character : public Entity, public IInputObject {
private:
    float _health;
    bool _activated;
    GunType _gun_type = GunType::NONE;
    SDL_Texture* _gun_sprite;
    Vector2 _direction = ZERO;
    BulletBuff _gun_buffed = BulletBuff(INFINITY, BulletBuffType::NONE);
    std::vector<CharBuff> _buff_list;


public:
    Character(Vector2 position, SDL_Texture* sprite, float speed, float health, bool activate);
    HitBox* get_collision();
    void shoot() override;
    void move(Vector2 direction) override;
    void update(float delta_time) override;
    void collide(ICollidable& object) override;
    void remove_buff(CharBuff& buff, CharBuffType buff_type);
};
