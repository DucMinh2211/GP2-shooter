#pragma once

#include "ICollidable.h"
#include "IUpdatable.h"
#include "IRenderable.h"
#include <vector>

// Forward declarations
class HitBox;
class Vector2;

class Obstacle : public ICollidable, public IUpdatable, public IRenderable {
protected:
    Vector2 _position;
    SDL_Texture* _sprite;
    std::vector<HitBox*> _hitbox_list;

public:
    Obstacle(Vector2 position, SDL_Texture* sprite, std::vector<HitBox*> hitbox_list) : _position(position), _sprite(sprite), _hitbox_list(hitbox_list){}
    virtual ~Obstacle() = default;
    std::vector<HitBox*>& get_hitboxes() override { return this->_hitbox_list; }
    Vector2 get_position() const { return this->_position; }
    const SDL_Texture* get_sprite() const { return this->_sprite; };
    virtual void update(float delta_time) override = 0;
    virtual void collide(ICollidable* object) override = 0;
    virtual void render(SDL_Renderer* renderer) override = 0;
};
