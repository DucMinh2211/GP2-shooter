#pragma once

#include "Obstacle.h"
#include "CharBuff.h"
#include "BulletBuff.h"
#include <SDL2/SDL_render.h>
#include <variant>

// forward decl

class BuffItem : public Obstacle {
private:
    std::variant<CharBuffType, BulletBuffType> _buff_type;
    bool _is_consumed = false;
    float _life_timer = 20.0f; // seconds before auto-disappear
public:
    BuffItem(Vector2 position, SDL_Texture *sprite, std::variant<CharBuffType, BulletBuffType> buff_type);
    ~BuffItem();
    std::variant<CharBuffType, BulletBuffType> get_buff_type() { return this->_buff_type; }
    bool is_consumed() const { return _is_consumed; }
    void consume() { _is_consumed = true; }
    void collide(ICollidable* object) override;
    void update(float delta_time) override; // concrete override so vtable exists
    void render(SDL_Renderer* renderer) override;
};
