#pragma once

#include "Buff.h"
#include "Character.h"

// Placeholder for Enum
enum class BulletBuffType {};

class BulletBuff : public Buff {
private:
    BulletBuffType _type;
    Character* _char;

public:
    void activate() override;
    void deactivate() override;
};
