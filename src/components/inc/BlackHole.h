#pragma once

#include "Obstacle.h"
#include "components/inc/ICollidable.h"
#include "components/inc/Character.h"
#include "components/inc/Bullet.h"
#include "math/Vector2.h"

class BlackHole : public Obstacle {
private:
    Vector2 _position;    // tâm blackhole
    float _outer_radius;  // vùng hút
    float _inner_radius;  // lõi
    float _dps_outer;     // damage per second ngoài
    float _dps_inner;     // damage per second trong

public:
    BlackHole(Vector2 pos, float outer, float inner,
              float dps_outer = 5.0f, float dps_inner = 15.0f);

    void collide(ICollidable& object);

    // getter để debug vẽ hitbox
    Vector2 get_position() const { return _position; }
    float get_outer_radius() const { return _outer_radius; }
    float get_inner_radius() const { return _inner_radius; }
};
