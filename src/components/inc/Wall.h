#pragma once

#include "Obstacle.h"

class Wall : public Obstacle {
public:
    void collide(std::vector<HitBox> object) override;
};
