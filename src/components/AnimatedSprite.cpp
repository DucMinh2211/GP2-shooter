#include "inc/AnimatedSprite.h"

AnimatedSprite::AnimatedSprite(SDL_Renderer* renderer, const std::string& spritesheet,
                               int frameWidth, int frameHeight, int frameCount, int frameTime, int columns)
    : currentFrame(0), frameTime(frameTime), timer(0.0f), lastUpdate(SDL_GetTicks()),
      frameWidth(frameWidth), frameHeight(frameHeight) {

    SDL_Surface* surf = IMG_Load(spritesheet.c_str());
    if (!surf) {
        SDL_Log("Failed to load %s: %s", spritesheet.c_str(), IMG_GetError());
    }
    texture = SDL_CreateTextureFromSurface(renderer, surf);
    SDL_FreeSurface(surf);

    // Build clips row-major across columns
    int rows = (frameCount + columns - 1) / columns;
    int created = 0;
    for (int r = 0; r < rows && created < frameCount; ++r) {
        for (int c = 0; c < columns && created < frameCount; ++c) {
            SDL_Rect clip = { c * frameWidth, r * frameHeight, frameWidth, frameHeight };
            clips.push_back(clip);
            ++created;
        }
    }
}

AnimatedSprite::~AnimatedSprite() {
    SDL_DestroyTexture(texture);
}

void AnimatedSprite::update(float deltaTime) {
    timer += deltaTime * 1000.0f; // deltaTime is seconds -> ms
    if (timer >= frameTime && !clips.empty()) {
        currentFrame = (currentFrame + 1) % clips.size();
        timer = 0.0f;
    }
}

void AnimatedSprite::render(SDL_Renderer* renderer, int x, int y, int scale, double angle) {
    if (clips.empty() || !texture) return;
    SDL_Point center = { 7, 8 };
    SDL_Rect dst = { x + 5, y , frameWidth * scale, frameHeight * scale };
    SDL_RenderCopyEx(renderer, texture, &clips[currentFrame], &dst, angle, &center, SDL_FLIP_NONE);
}
