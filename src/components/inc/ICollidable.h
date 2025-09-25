#pragma once

#include <vector>

// Forward Declaration
class HitBox;

class ICollidable {
public:
    virtual ~ICollidable() = default;
    virtual void collide(std::vector<HitBox> hitbox_list) = 0;
};
