#include "inc/InputHandler.h"
#include "inc/IInputObject.h"
#include "math/Vector2.h"
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_keycode.h>
#include <vector>

InputHandler::InputHandler(InputSet input_set, IInputObject* activated_char, IInputObject* unactivated_char) : _input_set(input_set), _activated_char(activated_char), _unactivated_char(unactivated_char) {
    if (this->_activated_char) {
        this->_activated_char->set_activate(true);
        this->_activated_char->set_input_set((int)this->_input_set);
    }
    if (this->_unactivated_char) {
        this->_unactivated_char->set_input_set((int)this->_input_set);
    }
}

void InputHandler::handle_event(SDL_Event& event, std::vector<Bullet*>& bullet_list, ResourceManager& resource_manager) {
    if (event.type == SDL_KEYDOWN || event.type == SDL_KEYUP) {
        bool key_down = (event.type == SDL_KEYDOWN);

        switch (_input_set) {
            case INPUT_1:
                switch (event.key.keysym.sym) {
                    case SDLK_w: _up = key_down; break;
                    case SDLK_s: _down = key_down; break;
                    case SDLK_a: _left = key_down; break;
                    case SDLK_d: _right = key_down; break;
                    case SDLK_LSHIFT:
                        if (key_down) {
                            if (!_unactivated_char) break; // nothing to swap with
                            if (this->_activated_char) {
                                this->_activated_char->set_activate(false);
                                this->_activated_char->set_direction(ZERO);
                            }
                            IInputObject* tmp_char = _activated_char;
                            _activated_char = _unactivated_char;
                            _unactivated_char = tmp_char;
                            if (this->_activated_char) this->_activated_char->set_activate(true);
                        }
                        break;
                    case SDLK_SPACE:
                        if (key_down) { _activated_char->shoot(bullet_list, resource_manager); }
                        break;
                }
                break;
            case INPUT_2:
                switch (event.key.keysym.sym) {
                    case SDLK_UP: _up = key_down; break;
                    case SDLK_DOWN: _down = key_down; break;
                    case SDLK_LEFT: _left = key_down; break;
                    case SDLK_RIGHT: _right = key_down; break;
                    case SDLK_RSHIFT:
                        if (key_down) {
                                if (!_unactivated_char) break; // nothing to swap with
                                if (this->_activated_char) {
                                    this->_activated_char->set_activate(false);
                                    this->_activated_char->set_direction(ZERO);
                                }
                                IInputObject* tmp_char = _activated_char;
                                _activated_char = _unactivated_char;
                                _unactivated_char = tmp_char;
                                if (this->_activated_char) this->_activated_char->set_activate(true);
                        }
                        break;
                    case SDLK_RETURN: // Enter key
                        if (key_down) { _activated_char->shoot(bullet_list, resource_manager); }
                        break;
                }
                break;
        }
    }
}

void InputHandler::update(float delta_time) {
    Vector2 direction = ZERO;
    if (_up) { direction.y -= 1; }
    if (_down) { direction.y += 1; }
    if (_left) { direction.x -= 1; }
    if (_right) { direction.x += 1; }

    if (direction.length_squared() > 0) {
        direction.normalize();
    }

    if (_activated_char) _activated_char->set_direction(direction);
}
