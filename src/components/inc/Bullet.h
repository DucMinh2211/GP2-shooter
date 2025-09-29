#pragma once

#include "Entity.h"
#include "ICollidable.h"
#include "BulletBuff.h"
#include "SDL2/SDL_render.h"
#include "IRenderable.h"
#include "Rect.h" // Add this include for Rect

class Bullet : public Entity, public IRenderable {
public:
    Bullet(Vector2 position, SDL_Texture* sprite, float speed, float damage, Vector2 init_direction, BulletBuffType buffed);
    float get_damage() { return this->_damage; }
    void update(float delta_time) override;
    void collide(ICollidable& object) override;
    void add_hitbox(HitBox* hitbox);
    void update_hitboxes();
    const std::vector<HitBox*>& get_hitboxes() const { return _hitbox_list; }
    void render(SDL_Renderer* renderer) override;

    // Buff helpers
    bool isBouncing() const { return _buffed == BulletBuffType::BOUNCING; }
    bool isExploding() const { return _buffed == BulletBuffType::EXPLODING; }
    bool isPiercing() const { return _buffed == BulletBuffType::PIERCING; }
    void setBuff(BulletBuffType buff) { _buffed = buff; }
    BulletBuffType getBuff() const { return _buffed; }
    Vector2 get_init_direction() const { return _init_direction; }

private:
    float _damage;
    Vector2 _init_direction;
    BulletBuffType _buffed;
    // _hitbox_list is inherited from Entity
};
