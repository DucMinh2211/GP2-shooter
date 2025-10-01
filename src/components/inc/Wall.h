#pragma once

#include "Obstacle.h"
#include "ICollidable.h"

class Wall : public Obstacle {
public:
    Wall(Vector2 position, SDL_Texture* sprite);
    ~Wall();
    void collide(ICollidable& object) override;
    void update(float delta_time) override;
    void render(SDL_Renderer* renderer) override;
};
