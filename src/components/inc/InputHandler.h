#pragma once

#include <vector>

// Forward declarations
class Character;
union SDL_Event;

class InputHandler {
private:
    Character& _activated_char;
    std::vector<Character*> _char_list;
public:
    void handle_event(SDL_Event& event);
    InputHandler(Character& char_, std::vector<Character*> char_list);
};
