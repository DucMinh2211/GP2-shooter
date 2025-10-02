#include "inc/Smoke.h"
#include <SDL2/SDL.h>

Smoke::Smoke(SDL_Renderer* renderer, const std::string& sheetPath, Vector2 pos, int frameW, int frameH, int frameCount, int frameTime, int columns)
    : Obstacle(pos, nullptr, {}), elapsed(0.0f), finished(false) {
    anim = new AnimatedSprite(renderer, sheetPath, frameW, frameH, frameCount, frameTime, columns);
    totalDurationMs = frameCount * frameTime;
}

Smoke::~Smoke() {
    delete anim;
}

void Smoke::update(float dt) {
    if (finished) return;
    elapsed += dt * 1000.0f;
    anim->update(dt);
    if (elapsed >= totalDurationMs) finished = true;
}

void Smoke::render(SDL_Renderer* renderer) {
    if (finished) return;
    anim->render(renderer, (int)_position.x - 24, (int)_position.y - 24, 1, 0.0);
}
