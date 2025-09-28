#pragma once

#include <vector>

// Forward declarations
class IInputObject;
union SDL_Event;

class InputHandler {
private:
    IInputObject& _activated_char;
    std::vector<IInputObject*> _char_list;
public:
    void handle_event(SDL_Event& event);
    InputHandler(IInputObject& char_, std::vector<IInputObject*> char_list);
};
