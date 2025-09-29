#pragma once

#include "Buff.h"

// Forward declaration
class IBuffable;

enum class CharBuffType {
    HEALTH = 0,
    NUM = 1, // size of CharBuffType
};


class CharBuff : public Buff {
private:
    CharBuffType _type;
    IBuffable& _char;
    bool _activated = false;

public:
    static constexpr float DURATION_LIST[(size_t)CharBuffType::NUM] {
        15.0f, // HEALTH
    };

    CharBuff(float duration, CharBuffType type, IBuffable& char_) : Buff(duration), _type(type), _char(char_) {}
    void timer_end() override;
};
