#include "inc/Rect.h"
#include "inc/Circle.h"

// Implement the dispatcher for is_collide
bool Rect::is_collide(HitBox& hitbox) {
    // Try dynamic_cast to Rect
    if (auto* rect = dynamic_cast<Rect*>(&hitbox)) {
        return is_collide(*rect);
    }
    // Try dynamic_cast to Circle
    if (auto* circle = dynamic_cast<Circle*>(&hitbox)) {
        return is_collide(*circle);
    }
    // Unknown type, no collision
    return false;
}
