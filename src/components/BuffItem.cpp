#include <SDL2/SDL_render.h>
#include <SDL2/SDL_timer.h>
#include "inc/BuffItem.h"
#include "inc/OBB.h"
#include "inc/Character.h"
#include <typeinfo>


BuffItem::BuffItem(Vector2 position, SDL_Texture *sprite, std::variant<CharBuffType, BulletBuffType> buff_type) : Obstacle(position, sprite, {}), _buff_type(buff_type) {
    int w, h;
    SDL_QueryTexture(_sprite, NULL, NULL, &w, &h);
    OBB* buff_item_hitbox = new OBB(position, Vector2(w / 2.0f, h / 2.0f), 0.0f);
    this->_hitbox_list.push_back(buff_item_hitbox);
}

BuffItem::~BuffItem() {
    for (HitBox* hitbox : _hitbox_list) {
        delete hitbox;
    }
    _hitbox_list.clear();
}

void BuffItem::render(SDL_Renderer* renderer) {
    if (!_sprite) return;

    int w, h;
    SDL_QueryTexture(_sprite, NULL, NULL, &w, &h);
    SDL_Rect dst_rect = {
        (int)(_position.x - w / 2.0f),
        (int)(_position.y - h / 2.0f),
        w,
        h
    };
    SDL_RenderCopy(renderer, _sprite, NULL, &dst_rect);
}

void BuffItem::collide(ICollidable* object) {
    // Only characters can consume buff items
    if (typeid(*object) == typeid(Character)) {
        for (auto* buff_hb : this->_hitbox_list) {
            for (auto* char_hb : object->get_hitboxes()) {
                if (buff_hb->is_collide(*char_hb)) {
                    object->collide(this);
                    _is_consumed = true;
                    return;
                }
            }
        }
    } else {
        // Allow double-dispatch for Character consuming buff
        object->collide(this);
    }
}

void BuffItem::update(float delta_time) {
    if (_is_consumed) return;
    _life_timer -= delta_time;
    if (_life_timer <= 0.0f) {
        _is_consumed = true;
    }
}
