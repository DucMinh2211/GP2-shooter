#include "inc/BasicAI.h"
#include "inc/Character.h"
#include "inc/Bullet.h"
#include "ResourceManager.h"
#include <cmath>
#include <iostream>

BasicAI::BasicAI(Character* ai_char, Character* player, std::vector<Bullet*>* bullets, ResourceManager* rm)
    : _ai_char(ai_char), _player(player), _bullets(bullets), _rm(rm), _shoot_timer(0.0f) {
    std::random_device rd;
    _rng.seed(rd());
}

void BasicAI::update(float delta_time) {
    if (!_ai_char || !_player) return;
    // Debug: print positions and delta_time
    std::cout << "BasicAI update: ai=(" << _ai_char->get_position().x << "," << _ai_char->get_position().y << ")"
              << " player=(" << _player->get_position().x << "," << _player->get_position().y << ") dt=" << delta_time << "\n";
    // Move towards player
    Vector2 dir = _player->get_position() - _ai_char->get_position();
    float len2 = dir.x*dir.x + dir.y*dir.y;
    if (len2 > 1.0f) {
        float len = std::sqrt(len2);
        dir.x /= len; dir.y /= len;
        _ai_char->set_direction(dir);
    } else {
        _ai_char->set_direction(Vector2(0,0));
    }

    // Shooting: simple cooldown
    _shoot_timer -= delta_time;
    if (_shoot_timer <= 0.0f) {
        // ask character to shoot (uses ResourceManager to create bullet texture)
        if (_bullets && _rm) {
            _ai_char->shoot(*_bullets, *_rm);
        }
        _shoot_timer = 1.0f + (std::uniform_real_distribution<float>(0.0f, 1.5f)(_rng));
    }
}
