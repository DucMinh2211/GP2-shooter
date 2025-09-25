#pragma once

#include "HitBox.h"
#include "math/Vector2.h"


class Circle : public HitBox {
private:
    float _radius;
    Vector2* _local_pos;

public:
    bool is_collide(HitBox& hitbox) override;
};
