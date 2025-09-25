#pragma once

#include "HitBox.h"
#include <SDL2/SDL_rect.h>

// Forward declaration
struct Vector2;

class Rect : public HitBox {
private:
    SDL_Rect _rect;
    Vector2* _local_pos;

public:
    bool is_collide(HitBox& hitbox) override;
};
