#pragma once

#include "IUpdatable.h"
#include <algorithm>

class Buff : public IUpdatable {
protected:
    float _duration;
    float _timer = 0.0f;
public:
    Buff(float duration) : _duration(duration) {}
    virtual ~Buff() = default;
    void update(float delta_time) override {
        if(this->_timer > 0.0f) {
            this->_timer -= delta_time;
            this->_timer = std::max(this->_timer, 0.0f);
        }
        else this->timer_end();
    }
    void timer_start() {
        this->_timer = this->_duration;
    }
    virtual void timer_end() = 0;
    bool is_activated();
};
