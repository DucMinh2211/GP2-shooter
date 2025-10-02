#include "inc/OBB.h"
#include "inc/Circle.h"
#include <cmath>

// Constructor
OBB::OBB(Vector2 center, Vector2 halfSize, float angle)
    : HitBox(center), _center(center), _halfSize(halfSize), _angle(angle) {}

// Update position + angle
void OBB::set_transform(const Vector2& center, float angle) {
    _center = center;
    _angle = angle;
    _local_pos = center;
}

// Lấy 4 đỉnh của OBB sau khi xoay
std::vector<Vector2> OBB::get_corners() const {
    std::vector<Vector2> corners;
    corners.reserve(4);

    std::vector<Vector2> localCorners = {
        {-_halfSize.x, -_halfSize.y},
        { _halfSize.x, -_halfSize.y},
        { _halfSize.x,  _halfSize.y},
        {-_halfSize.x,  _halfSize.y}
    };

    float cosA = cos(_angle);
    float sinA = sin(_angle);

    for (auto& c : localCorners) {
        float x = c.x * cosA - c.y * sinA;
        float y = c.x * sinA + c.y * cosA;
        corners.push_back(Vector2(x, y) + _center);
    }

    return corners;
}

// Debug draw
void OBB::debug_draw(SDL_Renderer* renderer, SDL_Color color) {
    auto corners = get_corners();
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);

    for (int i = 0; i < 4; i++) {
        Vector2 p1 = corners[i];
        Vector2 p2 = corners[(i + 1) % 4];
        SDL_RenderDrawLine(renderer, (int)p1.x, (int)p1.y, (int)p2.x, (int)p2.y);
    }
}

static void project_onto_axis(const std::vector<Vector2>& corners, const Vector2& axis, float& min, float& max) {
    min = max = Vector2::dot(axis,corners[0]);
    for (size_t i = 1; i < corners.size(); ++i) {
        float p = Vector2::dot(axis,corners[i]);
        if (p < min) min = p;
        if (p > max) max = p;
    }
}

// Kiểm tra OBB vs OBB bằng SAT
bool OBB::is_collide(OBB& other) {
    std::vector<Vector2> cornersA = get_corners();
    std::vector<Vector2> cornersB = other.get_corners();

    // Lấy các edge làm trục (normal)
    std::vector<Vector2> axes;
    for (int i = 0; i < 4; i++) {
        Vector2 edge = cornersA[(i + 1) % 4] - cornersA[i];
        axes.push_back(Vector2(-edge.y, edge.x)); // normal
    }
    for (int i = 0; i < 4; i++) {
        Vector2 edge = cornersB[(i + 1) % 4] - cornersB[i];
        axes.push_back(Vector2(-edge.y, edge.x));
    }

    // Chuẩn hóa axis
    for (auto& axis : axes) {
        float len = std::sqrt(axis.x * axis.x + axis.y * axis.y);
        if (len > 0) axis = axis * (1.0f / len);
    }

    // Kiểm tra projection
    for (auto& axis : axes) {
        float minA, maxA, minB, maxB;
        project_onto_axis(cornersA, axis, minA, maxA);
        project_onto_axis(cornersB, axis, minB, maxB);

        // Nếu không giao nhau trên trục này -> không va chạm
        if (maxA < minB || maxB < minA)
            return false;
    }

    // Nếu tất cả trục giao nhau -> va chạm
    return true;
}

// override HitBox
bool OBB::is_collide(HitBox& other) {
    if (auto* obb = dynamic_cast<OBB*>(&other))
        return is_collide(*obb);
    if (auto* circle = dynamic_cast<Circle*>(&other))
        return is_collide(*circle);
    return false; // future: có thể thêm Circle vs OBB
}

bool OBB::is_collide(Circle& circle) {
    // Vector từ OBB center tới Circle center
    Vector2 d = circle.get_center() - _center;

    // Tính cos & sin của -_angle để đưa về local space
    float c = std::cos(-_angle);
    float s = std::sin(-_angle);

    // Circle center trong local space của OBB
    float localX = d.x * c - d.y * s;
    float localY = d.x * s + d.y * c;

    // Clamp vào box [-halfSize, halfSize]
    float closestX = std::max(-_halfSize.x, std::min(localX, _halfSize.x));
    float closestY = std::max(-_halfSize.y, std::min(localY, _halfSize.y));

    // Vector từ circle center (local) đến điểm gần nhất
    float dx = localX - closestX;
    float dy = localY - closestY;

    return (dx * dx + dy * dy) <= (circle.get_radius() * circle.get_radius());
}