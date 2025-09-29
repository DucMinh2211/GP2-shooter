#pragma once

class ICollidable {
public:
    virtual ~ICollidable() = default;
    virtual void collide(ICollidable& object) = 0;
};
