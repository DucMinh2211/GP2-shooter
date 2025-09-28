#pragma once

// forward declaration
class Vector2;

class IInputObject {
public:
    virtual void move(Vector2 direction) = 0;
    virtual void shoot() = 0;
};
