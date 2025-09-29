#pragma once

#include "math/Vector2.h"
#include "SDL2/SDL.h"

class HitBox {
protected:
    Vector2 _local_pos;
public:
    HitBox(Vector2 local_pos) : _local_pos(local_pos) {}
    Vector2 get_local_pos() const {
        return this->_local_pos;
    }
    virtual void debug_draw(SDL_Renderer* renderer, SDL_Color color) = 0;
    virtual ~HitBox() = default;
    virtual bool is_collide(HitBox& hitbox) = 0;
};
