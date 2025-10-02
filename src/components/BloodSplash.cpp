#include "inc/BloodSplash.h"
#include "inc/Character.h"
#include <SDL2/SDL.h>

BloodSplash::BloodSplash(SDL_Renderer* renderer, const std::string& sheetPath, Vector2 pos, int frameW, int frameH, int frameCount, int frameTime, int columns)
    : Obstacle(pos, nullptr, {}), elapsed(0.0f), finished(false) {
    anim = new AnimatedSprite(renderer, sheetPath, frameW, frameH, frameCount, frameTime, columns);
    totalDurationMs = frameCount * frameTime;
}

BloodSplash::~BloodSplash() {
    delete anim;
}

void BloodSplash::update(float dt) {
    if (finished) return;
    elapsed += dt * 1000.0f;
    anim->update(dt);
    if (elapsed >= totalDurationMs) finished = true;
}

void BloodSplash::render(SDL_Renderer* renderer) {
    if (finished) return;
    anim->render(renderer, (int)_position.x - 12, (int)_position.y - 12, 1, 0.0);
}

