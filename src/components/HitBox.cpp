#include "inc/Rect.h"
#include "inc/Circle.h"
#include <cmath>

// Helper: clamp value between min and max
static float clamp(float value, float min, float max) {
    if (value < min) return min;
    if (value > max) return max;
    return value;
}

// Rect-Rect collision
bool Rect::is_collide(Rect& other) {
    SDL_Rect a = this->get_rect();
    SDL_Rect b = other.get_rect();
    return (a.x < b.x + b.w && a.x + a.w > b.x &&
            a.y < b.y + b.h && a.y + a.h > b.y);
}

// Rect-Circle collision
bool Rect::is_collide(Circle& circle) {
    SDL_Rect r = this->get_rect();
    Vector2 c = circle.get_local_pos();
    float radius = circle.get_radius();
    float closestX = clamp(c.x, r.x, r.x + r.w);
    float closestY = clamp(c.y, r.y, r.y + r.h);
    float dx = c.x - closestX;
    float dy = c.y - closestY;
    return (dx * dx + dy * dy) <= (radius * radius);
}

// Circle-Rect collision (calls Rect-Circle)
bool Circle::is_collide(Rect& rect) {
    return rect.is_collide(*this);
}

// Circle-Circle collision
bool Circle::is_collide(Circle& other) {
    Vector2 a = this->get_local_pos();
    Vector2 b = other.get_local_pos();
    float r = this->get_radius() + other.get_radius();
    float dx = a.x - b.x;
    float dy = a.y - b.y;
    return (dx * dx + dy * dy) <= (r * r);
}
