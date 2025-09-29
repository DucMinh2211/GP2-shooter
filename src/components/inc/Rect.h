#pragma once

#include "HitBox.h"
#include <SDL2/SDL.h>

// forward declaration
class Circle;

class Rect : public HitBox {
private:
    SDL_Rect _rect;
    float _angle; 

public:
    Rect(Vector2 local_pos, SDL_Rect rect, float angle = 0.0f) : HitBox(local_pos), _rect(rect), _angle(angle) {}
    SDL_Rect get_rect() const {
        return this->_rect;
    }
    void set_rect(const SDL_Rect& rect) {
        this->_rect = rect;
        this->_local_pos = Vector2(rect.x, rect.y);
    }
    void debug_draw(SDL_Renderer* renderer, SDL_Color color) override;
    float get_angle() const { return _angle; }
    void set_angle(float angle) { _angle = angle; }
    using HitBox::is_collide;
    bool is_collide(Rect& rect);
    bool is_collide(Circle& circle);
    bool is_collide(HitBox& hitbox) override;
};
