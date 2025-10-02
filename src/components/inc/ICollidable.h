#pragma once
#include "HitBox.h"
#include <vector>

class ICollidable {
public:
    virtual ~ICollidable() = default;
    virtual void collide(ICollidable* object) = 0;
    virtual std::vector<HitBox*>& get_hitboxes() = 0;
};
