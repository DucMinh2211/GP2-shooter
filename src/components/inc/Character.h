#pragma once

#include "Entity.h"
#include "BulletBuff.h"
#include "CharBuff.h"

// Placeholders for Enums
enum class GunType {};

class Character : public Entity {
public:
    Vector2 get_position();
    SDL_Texture* get_sprite();
    HitBox* get_collision();
    void shoot();
    void move(float dx, float dy);
    void update(float delta_time) override;
    void collide(std::vector<HitBox> object) override;
    void add_buff(CharBuff& buff, CharBuffType buff_type);
    void remove_buff(CharBuff& buff, CharBuffType buff_type);
    void change_bullet_buff(BulletBuff& bullet_buff, BulletBuffType buff_type);

private:
    float _health;
    SDL_Texture* _gun_sprite;
    bool _activated;
    GunType _gun_type;
    std::vector<CharBuff*> _buff_list;
    BulletBuffType _gun_buffed;
};
