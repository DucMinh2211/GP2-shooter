#pragma once

#include "Buff.h"

// Forward declaration
class IBuffable;

enum class CharBuffType {
    HEALTH = 0,
    SPEED = 1,
    NUM = 2, // size of CharBuffType
};


class CharBuff : public Buff {
private:
    CharBuffType _type;
    IBuffable& _char;

public:
    bool _activated = false;
    static constexpr float DURATION_LIST[(size_t)CharBuffType::NUM] {
        15.0f, // HEALTH
    };

    CharBuff(float duration, CharBuffType type, IBuffable& char_) : Buff(duration), _type(type), _char(char_) {}
    void timer_end() override;
};
