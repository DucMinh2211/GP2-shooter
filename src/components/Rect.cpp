#include "inc/Rect.h"
#include "inc/Circle.h"
#include "SDL2/SDL.h"

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
void Rect::debug_draw(SDL_Renderer* renderer, SDL_Color color) {
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);

    // Lấy tâm của rect
    float cx = _rect.x + _rect.w / 2.0f;
    float cy = _rect.y + _rect.h / 2.0f;

    // Chuyển angle sang radian
    float rad = _angle * M_PI / 180.0f;

    // 4 góc ban đầu (chưa xoay)
    Vector2 corners[4] = {
        { _rect.x,         _rect.y },
        { _rect.x+_rect.w, _rect.y },
        { _rect.x+_rect.w, _rect.y+_rect.h },
        { _rect.x,         _rect.y+_rect.h }
    };

    // Sau khi xoay quanh tâm
    SDL_Point points[5];
    for (int i = 0; i < 4; i++) {
        float dx = corners[i].x - cx;
        float dy = corners[i].y - cy;
        float rx = dx * cos(rad) - dy * sin(rad);
        float ry = dx * sin(rad) + dy * cos(rad);
        points[i].x = static_cast<int>(cx + rx);
        points[i].y = static_cast<int>(cy + ry);
    }
    points[4] = points[0]; // đóng polygon

    SDL_RenderDrawLines(renderer, points, 5);
}
