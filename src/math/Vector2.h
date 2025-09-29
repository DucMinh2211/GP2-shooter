#pragma once
#include <cmath>

class Vector2 {
public:
    float x;
    float y;

    constexpr Vector2(float x = 0.0f, float y = 0.0f) : x(x), y(y) {}

    // Member functions
    Vector2& add(const Vector2& vec) {
        x += vec.x;
        y += vec.y;
        return *this;
    }

    Vector2& subtract(const Vector2& vec) {
        x -= vec.x;
        y -= vec.y;
        return *this;
    }

    Vector2& multiply(float scalar) {
        x *= scalar;
        y *= scalar;
        return *this;
    }

    Vector2& divide(float scalar) {
        x /= scalar;
        y /= scalar;
        return *this;
    }

    float length() const {
        return std::sqrt(x * x + y * y);
    }

    float length_squared() const {
        return x * x + y * y;
    }

    Vector2& normalize() {
        float len = length();
        if (len > 0) {
            x /= len;
            y /= len;
        }
        return *this;
    }

    static float dot(const Vector2& a, const Vector2& b) {
        return a.x * b.x + a.y * b.y;
    }

    // Operator overloads
    Vector2 operator-() const { return Vector2(-x, -y); }

    Vector2 operator+(const Vector2& other) const { return Vector2(x + other.x, y + other.y); }
    Vector2 operator-(const Vector2& other) const { return Vector2(x - other.x, y - other.y); }
    Vector2 operator*(float scalar) const { return Vector2(x * scalar, y * scalar); }
    Vector2 operator/(float scalar) const { return Vector2(x / scalar, y / scalar); }

    Vector2& operator+=(const Vector2& other) { return add(other); }
    Vector2& operator-=(const Vector2& other) { return subtract(other); }
    Vector2& operator*=(float scalar) { return multiply(scalar); }
    Vector2& operator/=(float scalar) { return divide(scalar); }

    bool operator==(const Vector2& other) const { return x == other.x && y == other.y; }
    bool operator!=(const Vector2& other) const { return !(*this == other); }
};

// Allow scalar multiplication from the left
inline Vector2 operator*(float scalar, const Vector2& vec) {
    return vec * scalar;
}

static constexpr inline Vector2 ZERO{0.0f, 0.0f};
