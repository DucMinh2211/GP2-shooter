#pragma once

#include "Obstacle.h"

// Forward declaration
class Character;

class BlackHole : public Obstacle {
private:
    float _outer_radius;
    float _inner_radius;
    float _dps_outer;
    float _dps_inner;

    // Lists to track colliding characters
    std::vector<Character*> _characters_in_outer_zone;
    std::vector<Character*> _characters_in_inner_zone;

public:
    BlackHole(Vector2 pos, SDL_Texture* sprite, float outer_radius, float inner_radius,
              float dps_outer = 5.0f, float dps_inner = 15.0f);
    ~BlackHole();

    void collide(ICollidable& object) override;
    void update(float delta_time) override;
    void render(SDL_Renderer* renderer) override;

    // getter to debug draw hitbox
    float get_outer_radius() const { return _outer_radius; }
    float get_inner_radius() const { return _inner_radius; }
};
