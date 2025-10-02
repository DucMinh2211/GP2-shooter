#pragma once

#include "AnimatedSprite.h"
#include "math/Vector2.h"
#include "Obstacle.h"

class Explosion : public Obstacle {
public:
    Explosion(SDL_Renderer* renderer, const std::string& sheetPath, Vector2 pos,
              int frameW, int frameH, int frameCount, int frameTime, int columns = 1, float damage = 25.0f);
    ~Explosion();

    float get_damage() const { return this->_damage; }

    void update(float dt) override;
    void render(SDL_Renderer* renderer) override;
    void collide(ICollidable* object) override;

    bool is_finished() const { return finished; }

    bool damageable = true;

private:
    AnimatedSprite* anim;
    float elapsed;
    float totalDurationMs;
    bool finished;
    float _damage;
};
