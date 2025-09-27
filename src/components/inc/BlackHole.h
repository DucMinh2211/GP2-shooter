#pragma once

#include "Obstacle.h"
#include "components/inc/ICollidable.h"

class BlackHole : public Obstacle {
public:
    void collide(ICollidable& object) override;
};
