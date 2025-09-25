#pragma once

#include "HitBox.h"
#include <vector>

class ICollidable {
public:
    virtual ~ICollidable() = default;
    virtual void collide(std::vector<HitBox> hitbox_list) = 0;
};
