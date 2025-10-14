#include "inc/BulletBuff.h"

void BulletBuff::timer_end() {
    this->_type = BulletBuffType::NONE;
}

void BulletBuff::set_type(BulletBuffType type) {
    this->_type = type;
    this->_duration = DURATION_LIST[(size_t)type];
    // start/reset timer so the new buff takes effect immediately
    this->timer_start();
}
