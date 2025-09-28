#pragma once

#include "HitBox.h"

// forward declaration
class Rect;

class Circle : public HitBox {
private:
    float _radius;

public:
    Circle(Vector2 local_pos, float radius) : HitBox(local_pos), _radius(radius) {}
    float get_radius() const {
        return this->_radius;
    }
    using HitBox::is_collide;
    bool is_collide(Rect& rect);
    bool is_collide(Circle& circle);
};
