#pragma once

#include "HitBox.h"
#include <SDL2/SDL.h>

// forward declaration
class OBB;

class Circle : public HitBox {
private:
    float _radius;

public:
    Circle(Vector2 local_pos, float radius) : HitBox(local_pos), _radius(radius) {}

    float get_radius() const { return _radius; }
    void set_radius(float r) { _radius = r; }

    Vector2 get_center() const { return _local_pos; }
    void set_center(const Vector2& pos) { _local_pos = pos; }

    void debug_draw(SDL_Renderer* renderer, SDL_Color color) override;

    using HitBox::is_collide;
    bool is_collide(Circle& circle);
    bool is_collide(OBB& obb);

    bool is_collide(HitBox& hitbox) override;
};
