#pragma once

#include "SDL_render.h"
class IRenderable {
public:
    virtual void render(SDL_Renderer* renderer) = 0;
};
