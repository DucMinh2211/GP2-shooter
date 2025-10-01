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
public:
    std::variant<CharBuffType, BulletBuffType> get_buff_type();
    void collide(ICollidable& object) override;
    void update(float delta_time) override;
    void render(SDL_Renderer* renderer) override;
};
