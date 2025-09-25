#pragma once

#include "Obstacle.h"

class Wall : public Obstacle {
public:
    void collide(Rect& object) override;
};
