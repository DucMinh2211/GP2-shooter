#pragma once

#include "IUpdatable.h"
#include <random>
#include <vector>

// Forward declarations
class Character;
class Bullet;
class ResourceManager;

class BasicAI : public IUpdatable {
private:
    Character* _ai_char;
    Character* _player;
    std::vector<Bullet*>* _bullets;
    ResourceManager* _rm;
    float _shoot_timer;
    std::mt19937 _rng;
public:
    BasicAI(Character* ai_char, Character* player, std::vector<Bullet*>* bullets, ResourceManager* rm);
    void update(float delta_time) override;
    ~BasicAI() = default;
};
