#pragma once

#include "Obstacle.h"

class BlackHole : public Obstacle {
public:
    void collide(std::vector<HitBox> object) override;
};
