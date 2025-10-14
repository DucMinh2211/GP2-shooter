#pragma once

#include "AnimatedSprite.h"
#include "math/Vector2.h"
#include "Obstacle.h"
#include <unordered_set>

class Explosion : public Obstacle {
public:
    Explosion(SDL_Renderer* renderer, const std::string& sheetPath, Vector2 pos,
              int frameW, int frameH, int frameCount, int frameTime, int columns = 1, float damage = 25.0f, int owner_team = -1);
    ~Explosion();

    float get_damage() const { return this->_damage; }

    void update(float dt) override;
    void render(SDL_Renderer* renderer) override;
    void collide(ICollidable* object) override;

    bool is_finished() const { return finished; }

    // explosion may damage multiple characters, but each character should be
    // damaged at most once. The set below tracks which characters were hit.

private:
    AnimatedSprite* anim;
    float elapsed;
    float totalDurationMs;
    bool finished;
    float _damage;
    int _owner_team;
    std::unordered_set<class Character*> _damaged;
};
