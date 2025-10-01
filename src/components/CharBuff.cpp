#include "inc/CharBuff.h"
#include "inc/Character.h"

void CharBuff::timer_end() {
    this->_activated = false;
    this->_char.remove_buff(this->_type);
}
