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
};

const std::pmr::unordered_map<GunType, float> SHOOT_DELAY_MAP {
    {GunType::AK, 0.5},
    {GunType::PISTOL, 1},
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
    BulletBuff _gun_buffed = BulletBuff(INFINITY, BulletBuffType::NONE); // private buff state
    std::vector<CharBuff> _buff_list;
    Vector2 _last_move_vec = ZERO;
    AnimatedSprite* _idle_anim = nullptr;
    AnimatedSprite* _run_anim = nullptr;
    AnimatedSprite* _shoot_anim = nullptr;
    AnimatedSprite* _dead_anim = nullptr;

    AnimatedSprite* _current_anim = nullptr; // animation đang sử dụng
    float _angle = 0.0f; // góc xoay theo hướng di chuyển
    float _shoot_timer = 0.0f; // thời gian còn lại cho animation bắn
    float _shoot_duration = 0.5f; // tổng thời gian animation bắn (giây)


public:
    Character(Vector2 position, SDL_Texture *sprite, float speed, float health);
    float get_health() const { return this->_health; }
    BulletBuffType get_gun_buff_type() const { return _gun_buffed.getType(); }
    HitBox* get_collision();
    std::vector<HitBox*>& get_hitboxes() override { return _hitbox_list; }
    void shoot(std::vector<Bullet*>& bullet_list, ResourceManager& resource_manager) override;
    void set_activate(bool activated) override;
    void set_input_set(int input_set) override;
    void set_direction(Vector2 direction) override;
    void update(float delta_time) override;
    void collide(ICollidable* object) override;
    void remove_buff(CharBuffType buff_type) override;
    void render(SDL_Renderer *renderer) override;
    void render_activated_circle(SDL_Renderer *renderer);

    bool is_dead() const { return this->_health <= 0; }

    // Public API for state modification
    void take_damage(float amount);
    void add_force(Vector2 force);
    int get_input_set() const { return _input_set; }
    void set_animations(AnimatedSprite* idle, AnimatedSprite* run, AnimatedSprite* shoot, AnimatedSprite* dead = nullptr) {
        _idle_anim = idle;
        _run_anim  = run;
        _shoot_anim = shoot;
        _dead_anim = dead;
        _current_anim = _idle_anim;
    }

    // Expose gun type accessors for game-mode logic
    GunType get_gun_type() const { return _gun_type; }
    void set_gun_type(GunType gt) { _gun_type = gt; _shoot_delay = SHOOT_DELAY_MAP.at(_gun_type); }
    // Bullet buff control (allow game code to set/clear a character's active bullet buff)
    void set_bullet_buff(BulletBuffType type) { _gun_buffed.set_type(type); }
    void clear_bullet_buff() { _gun_buffed.set_type(BulletBuffType::NONE); }
    // Debug helpers: report active bullet buff and active char buffs
    BulletBuffType get_active_bullet_buff() const { return _gun_buffed.getType(); }
    std::vector<CharBuffType> get_active_char_buffs() const;
};
