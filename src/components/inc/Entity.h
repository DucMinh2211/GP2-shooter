#pragma once

#include "ICollidable.h"
#include "IUpdatable.h"
#include "math/Vector2.h"
#include <SDL2/SDL_render.h>
#include <vector>

class Entity : public ICollidable, public IUpdatable {
protected:
    Vector2* _position;
    SDL_Texture* _sprite;
    float _speed;
    Vector2* _force;
    std::vector<HitBox*> _hitbox_list;

public:
    Vector2 get_position();
    SDL_Texture* get_sprite();
    void collide(std::vector<HitBox> hitbox_list) override;
    void update(float delta_time) override;
};
