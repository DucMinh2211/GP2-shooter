#pragma once

#include "Buff.h"
#include <cmath>

// Placeholder for Enum
enum class BulletBuffType {
    NONE = 0,
    FIRE = 1,
    EXPLOSION = 2,
    NUM = 3,
};

class BulletBuff : public Buff {
private:
    BulletBuffType _type;

public:
    static constexpr float DURATION_LIST[(size_t)BulletBuffType::NUM] {
        INFINITY,
        2.0,
        2.0,
    };
    BulletBuff(float duration, BulletBuffType type) : Buff(duration), _type(type) {}
    void timer_end() override;

};
