#pragma once

#include "Entity.h"
#include "BulletBuff.h"
#include "CharBuff.h"
#include "IInputObject.h"
#include "IRenderable.h"
#include <unordered_map>
#include <vector>
#include "AnimatedSprite.h"


// forward decl
class ResourceManager;

enum class GunType {
    PISTOL = 1,
    AK = 2,
    SHOTGUN = 3,
};

const std::pmr::unordered_map<GunType, float> SHOOT_DELAY_MAP {
    {GunType::AK, 0.1},
    {GunType::PISTOL, 0.3},
    {GunType::SHOTGUN, 0.5}
};

class IBuffable {
public:
    virtual void remove_buff(CharBuffType buff_type) = 0;
};

class Character : public Entity, public IInputObject, public IRenderable, public IBuffable {
private:
    int _input_set = 0;
    float _health;
    float _shoot_delay;
    bool _activated = false;
    GunType _gun_type = GunType::PISTOL;
    Vector2 _direction = ZERO;
    Vector2 _last_direction = {1.0f, 0.0f}; // last non-zero direction
    BulletBuff _gun_buffed = BulletBuff(INFINITY, BulletBuffType::NONE);
    std::vector<CharBuff> _buff_list;
    Vector2 _last_move_vec = ZERO;
    AnimatedSprite* _idle_anim = nullptr;
    AnimatedSprite* _run_anim = nullptr;
    AnimatedSprite* _shoot_anim = nullptr;

    AnimatedSprite* _current_anim = nullptr; // animation đang sử dụng
    float _angle = 0.0f; // góc xoay theo hướng di chuyển
    float _shoot_timer = 0.0f; // thời gian còn lại cho animation bắn
    float _shoot_duration = 0.5f; // tổng thời gian animation bắn (giây)


public:
    Character(Vector2 position, SDL_Texture *sprite, float speed, float health);
    HitBox* get_collision();
    std::vector<HitBox*>& get_hitboxes() override { return _hitbox_list; }
    void shoot(std::vector<Bullet*>& bullet_list, ResourceManager& resource_manager) override;
    void set_activate(bool activated) override;
    void set_input_set(int input_set) override;
    void set_direction(Vector2 direction) override;
    void update(float delta_time) override;
    void collide(ICollidable& object) override;
    void remove_buff(CharBuffType buff_type) override;
    void render(SDL_Renderer *renderer) override;
    void render_activated_circle(SDL_Renderer *renderer);

    // Public API for state modification
    void take_damage(float amount);
    void add_force(Vector2 force);
    void set_animations(AnimatedSprite* idle, AnimatedSprite* run, AnimatedSprite* shoot) {
        _idle_anim = idle;
        _run_anim  = run;
        _shoot_anim = shoot;
        _current_anim = _idle_anim;
    }
};
