#pragma once

class IUpdatable {
public:
    virtual ~IUpdatable() = default;
    virtual void update(float delta_time) = 0;
};
