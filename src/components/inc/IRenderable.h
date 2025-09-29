#pragma once

#include "SDL2/SDL_render.h"
class IRenderable {
public:
    virtual void render(SDL_Renderer* renderer) = 0;
};
