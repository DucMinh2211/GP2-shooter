#pragma once

class Vector2 {
private:
    float _x;
    float _y;

public:
    constexpr Vector2(float x, float y) : _x(x), _y(y) {}
};
static constexpr inline Vector2 ZERO{0.0f, 0.0f};
