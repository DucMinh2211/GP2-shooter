#pragma once

#include "HitBox.h"

// Forward Declaration
class Vector2;

class Circle : public HitBox {
private:
    float _radius;
    Vector2* _local_pos;

public:
    bool is_collide(HitBox& hitbox) override;
};
