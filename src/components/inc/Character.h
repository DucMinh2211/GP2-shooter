#pragma once

#include "Entity.h"
#include "BulletBuff.h"
#include "CharBuff.h"
#include "IInputObject.h"
#include "IRenderable.h"
#include <vector>

// forward decl
class ResourceManager;

enum class GunType {
    NONE = 0,
    PISTOL = 1,
    AK = 2,
    SHOTGUN = 3,
    NUM = 4, // size of guntype
};

class IBuffable {
public:
    virtual void remove_buff(CharBuffType buff_type) = 0;
};

class Character : public Entity, public IInputObject, public IRenderable, public IBuffable {
private:
    float _health;
    bool _activated;
    GunType _gun_type = GunType::NONE;
    SDL_Texture* _gun_sprite;
    Vector2 _direction = {1.0f, 0.0f};
    Vector2 _last_direction = {1.0f, 0.0f}; // last non-zero direction
    BulletBuff _gun_buffed = BulletBuff(INFINITY, BulletBuffType::NONE);
    std::vector<CharBuff> _buff_list;


public:
    Character(Vector2 position, SDL_Texture* sprite, float speed, float health, bool activate);
    HitBox* get_collision();
    std::vector<HitBox*>& get_hitboxes() override { return _hitbox_list; }
    void shoot(std::vector<Bullet*>& bullet_list, ResourceManager& resource_manager) override;
    void set_direction(Vector2 direction) override;
    void update(float delta_time) override;
    void collide(ICollidable& object) override;
    void remove_buff(CharBuffType buff_type) override;
    void render(SDL_Renderer* renderer) override;
};
