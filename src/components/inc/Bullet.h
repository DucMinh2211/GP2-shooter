#pragma once

#include "Entity.h"
#include "ICollidable.h"
#include "BulletBuff.h"
#include "SDL2/SDL_render.h"
#include "IRenderable.h"
#include "Rect.h" // Add this include for Rect
#include <vector>

class Explosion;

class Bullet : public Entity, public IRenderable {
public:
    Bullet(Vector2 position, SDL_Texture* sprite, float speed, float damage, Vector2 init_direction, BulletBuffType buffed, int team_id);
    float get_damage() { return this->_damage; }
    int get_team_id() { return this->_team_id; }
    void update(float delta_time) override;
    void collide(ICollidable* object) override;
    void add_hitbox(HitBox* hitbox);
    void update_hitboxes();
    std::vector<HitBox*>& get_hitboxes() override { return _hitbox_list; }
    void render(SDL_Renderer* renderer) override;
    void add_force(Vector2 force);
    void explode(std::vector<Explosion*>& explosion_list, SDL_Renderer* renderer);

    // Buff helpers
    bool isBouncing() const { return _buffed == BulletBuffType::BOUNCING; }
    bool isExploding() const { return _buffed == BulletBuffType::EXPLODING; }
    bool isPiercing() const { return _buffed == BulletBuffType::PIERCING; }
    void setBuff(BulletBuffType buff) { _buffed = buff; }
    BulletBuffType getBuff() const { return _buffed; }
    Vector2 get_init_direction() const { return _init_direction; }
    bool is_destroyed() const { return _is_destroyed; }
    void set_destroyed(bool destroyed = true) { _is_destroyed = destroyed; }
    ~Bullet();

private:
    const float _LIFE_TIME = 10.0;
    int _team_id;
    float _damage;
    float _life_timer = _LIFE_TIME;
    Vector2 _init_direction;
    BulletBuffType _buffed;
    bool _is_destroyed = false;
};
