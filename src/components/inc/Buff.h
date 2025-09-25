#pragma once

#include "IUpdatable.h"

class Buff : public IUpdatable {
protected:
    float _duration;
    float _timer;
    bool _activated;
public:
    virtual ~Buff() = default;
    void update(float delta_time) override;
    virtual void activate() = 0;
    bool is_activated();
    virtual void deactivate() = 0;
};
