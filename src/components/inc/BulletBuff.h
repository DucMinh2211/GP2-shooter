#pragma once

#include "Buff.h"

// Forward declaration
class Character;

// Placeholder for Enum
enum class BulletBuffType {};

class BulletBuff : public Buff {
public:
    void activate() override;
    void deactivate() override;
private:
    BulletBuffType _type;
    Character* _char;
};
