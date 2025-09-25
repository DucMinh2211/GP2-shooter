#pragma once

#include "Entity.h"
#include <vector>

// Forward declarations
class Texture;
class Rect;
class CharBuff;
class BulletBuff;
struct Vector2;

// Placeholders for Enums
enum class GunType {};
enum class BulletBuffType {};
enum class CharBuffType {};


class Character : public Entity {
public:
    Vector2 get_position();
    Texture* get_sprite();
    Rect* get_collision();
    void shoot();
    void move(float dx, float dy);
    void update(float delta_time);
    void collide(Rect& object) override;
    void add_buff(CharBuff& buff, CharBuffType buff_type);
    void remove_buff(CharBuff& buff, CharBuffType buff_type);
    void change_bullet_buff(BulletBuff& bullet_buff, BulletBuffType buff_type);

private:
    float _health;
    Texture* _gun_sprite;
    bool _activated;
    GunType _gun_type;
    std::vector<CharBuff*> _buff_list;
    BulletBuffType _gun_buffed;
};
