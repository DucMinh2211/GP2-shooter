#pragma once

#include <vector>

// forward declaration
class Vector2;
class Bullet;
class ResourceManager;

class IInputObject {
public:
    virtual void set_input_set(int input_set) = 0;
    virtual void set_direction(Vector2 direction) = 0; 
    virtual void set_activate(bool activated) = 0;
    virtual void shoot(std::vector<Bullet*>& bullet_list, ResourceManager& resource_manager) = 0;
};
