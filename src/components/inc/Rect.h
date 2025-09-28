#pragma once

#include "HitBox.h"
#include <SDL2/SDL_rect.h>

// forward declaration
class Circle;

class Rect : public HitBox {
private:
    SDL_Rect _rect;

public:
    Rect(Vector2 local_pos, SDL_Rect rect) : HitBox(local_pos), _rect(rect) {}
    SDL_Rect get_rect() const {
        return this->_rect;
    }
    using HitBox::is_collide;
    bool is_collide(Rect& rect);
    bool is_collide(Circle& circle);
};
