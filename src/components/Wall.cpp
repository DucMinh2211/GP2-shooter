#include "inc/Wall.h"
#include "inc/OBB.h"
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_error.h>
#include <iostream>

// Constructor: Automatically creates a hitbox based on the sprite's dimensions.
Wall::Wall(Vector2 position, SDL_Texture* sprite)
    : Obstacle(position, sprite, {}) { // Call base with an empty hitbox list initially.
    
    if (sprite) {
        int w, h;
        if (SDL_QueryTexture(sprite, NULL, NULL, &w, &h) != 0) {
            std::cerr << "Failed to query wall texture: " << SDL_GetError() << std::endl;
            return;
        }
        // Create an OBB hitbox based on the sprite size, centered at the wall's position.
        Vector2 half_size(w / 2.0f, h / 2.0f);
        OBB* wall_hitbox = new OBB(_position, half_size, 0.0f);
        this->_hitbox_list.push_back(wall_hitbox);
    }
}

// Destructor: Cleans up the hitboxes created in the constructor.
Wall::~Wall() {
    for (auto* hitbox : _hitbox_list) {
        delete hitbox;
    }
    _hitbox_list.clear();
}

// A wall doesn't react to a collision; the other object is responsible for the response.
void Wall::collide(ICollidable& object) {
    // Empty
}

// A stationary wall does not need to do anything during the update phase.
void Wall::update(float delta_time) {
    // Empty
}

// Renders the wall's sprite at its position.
void Wall::render(SDL_Renderer* renderer) {
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
