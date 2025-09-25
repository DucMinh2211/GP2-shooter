#pragma once

#include <vector>

// Forward declarations
class Character;
union SDL_Event;

class InputHandler {
public:
    void handle_event(SDL_Event& event);
private:
    Character* _activated_char;
    std::vector<Character*> _char_list;
};
