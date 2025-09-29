#pragma once

#include "Obstacle.h"
#include "ICollidable.h"

class Wall : public Obstacle {
public:
    void collide(ICollidable& object) override;
};
