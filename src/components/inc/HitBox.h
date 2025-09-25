#pragma once

class HitBox {
public:
    virtual ~HitBox() = default;
    virtual bool is_collide(HitBox& hitbox) = 0;
};
