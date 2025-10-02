#pragma once

#include "Buff.h"
#include <cmath>

// Placeholder for Enum
enum class BulletBuffType {
    NONE = 0,
    BOUNCING = 1,
    EXPLODING = 2,
    PIERCING = 3,
    NUM = 4,
};

class Bullet;
class BulletBuff : public Buff {
private:
    BulletBuffType _type;

public:
    static constexpr float DURATION_LIST[(size_t)BulletBuffType::NUM] {
        INFINITY, // NONE
        30.0,     // BOUNCING
        30.0,     // EXPLODING
        30.0,     // PIERCING
    };
    BulletBuff(float duration, BulletBuffType type) : Buff(duration), _type(type) {}
    void timer_end() override;
    BulletBuffType getType() const { return _type; }
    void set_type(BulletBuffType type);
};
