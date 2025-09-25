#pragma once

#include "Buff.h"

// Forward declaration
class Character;

// Placeholder for Enum
enum class CharBuffType {};

class CharBuff : public Buff {
public:
    void activate() override;
    void deactivate() override;
private:
    CharBuffType _type;
    Character* _char;
};
