#pragma once

#include "HitBox.h"

// Forward declaration
struct Vector2;

class Circle : public HitBox {
public:
    bool is_collide(HitBox& hitbox) override;
private:
    float _radius;
    Vector2* _local_pos; // Assuming Vector2 is a struct/class with x, y
};
