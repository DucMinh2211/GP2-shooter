#pragma once

#include "IUpdatable.h"
#include <algorithm>

class Buff : public IUpdatable {
protected:
    bool is_activated = false;
    float _duration;
    float _timer;
public:
    Buff(float duration) : _duration(duration) {}
    virtual ~Buff() = default;
    void update(float delta_time) override {
        if(this->_timer > 0.0f) {
            this->_timer -= delta_time;
            this->_timer = std::max(this->_timer, 0.0f);
        }
        else if (is_activated) {
            this->timer_end();
            is_activated = false;
        }
    }
    void timer_start() {
        this->_timer = this->_duration;
        is_activated = true;
    }
    virtual void timer_end() = 0;
};
