#pragma once

#include "Obstacle.h"

class BlackHole : public Obstacle {
public:
    void collide(Rect& object) override;
};
