#include "inc/Circle.h"
#include "inc/OBB.h"
#include <cmath>

// Circle vs Circle
bool Circle::is_collide(Circle& other) {
    float dx = _local_pos.x - other.get_center().x;
    float dy = _local_pos.y - other.get_center().y;
    float r = _radius + other.get_radius();
    return (dx * dx + dy * dy) <= (r * r);
}

// Circle vs OBB (gọi ngược lại cho code reuse)
bool Circle::is_collide(OBB& obb) {
    return obb.is_collide(*this);
}

// Dynamic dispatch
bool Circle::is_collide(HitBox& hitbox) {
    if (auto* c = dynamic_cast<Circle*>(&hitbox)) {
        return is_collide(*c);
    }
    else if (auto* o = dynamic_cast<OBB*>(&hitbox)) {
        return is_collide(*o);
    }
    return false;
}

// Debug draw circle
void Circle::debug_draw(SDL_Renderer* renderer, SDL_Color color) {
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    const int steps = 64;
    float angleStep = 2.0f * M_PI / steps;
    for (int i = 0; i < steps; i++) {
        float theta1 = i * angleStep;
        float theta2 = (i + 1) * angleStep;
        int x1 = (int)(_local_pos.x + cos(theta1) * _radius);
        int y1 = (int)(_local_pos.y + sin(theta1) * _radius);
        int x2 = (int)(_local_pos.x + cos(theta2) * _radius);
        int y2 = (int)(_local_pos.y + sin(theta2) * _radius);
        SDL_RenderDrawLine(renderer, x1, y1, x2, y2);
    }
}
