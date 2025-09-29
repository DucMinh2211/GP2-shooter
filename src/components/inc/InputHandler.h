#pragma once

#include "IUpdatable.h"
#include <vector>

// Forward declarations
class IInputObject;
class Bullet;
class ResourceManager;
union SDL_Event;

enum InputSet {
    INPUT_1,
    INPUT_2
};

class InputHandler : public IUpdatable {
private:
    InputSet _input_set;
    IInputObject& _activated_char;
    std::vector<IInputObject*> _char_list;

    bool _up = false;
    bool _down = false;
    bool _left = false;
    bool _right = false;

public:
    void handle_event(SDL_Event& event, std::vector<Bullet*>& bullet_list, ResourceManager& resource_manager);
    void update(float delta_time) override;
    InputHandler(InputSet input_set, IInputObject& char_, std::vector<IInputObject*> char_list);
};
