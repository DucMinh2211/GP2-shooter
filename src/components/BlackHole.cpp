#include "inc/BlackHole.h"
#include "inc/OBB.h"
#include <cmath>
#include <iostream>

BlackHole::BlackHole(Vector2 pos, float outer, float inner,
                     float dps_outer, float dps_inner)
    : _position(pos),
      _outer_radius(outer),
      _inner_radius(inner),
      _dps_outer(dps_outer),
      _dps_inner(dps_inner) {}

void BlackHole::collide(ICollidable& object) {
    for (auto* hbSelf : get_hitboxes()) {
        for (auto* hbOther : object.get_hitboxes()) {
            if (hbSelf->is_collide(*hbOther)) {
                std::cout << "[BlackHole] Collision!\n";
            }
        }
    }
}

