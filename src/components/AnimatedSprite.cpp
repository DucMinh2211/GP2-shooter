#include "inc/AnimatedSprite.h"

AnimatedSprite::AnimatedSprite(SDL_Renderer* renderer, const std::string& spritesheet,
                               int frameWidth, int frameHeight, int frameCount, int frameTime)
    : currentFrame(0), frameTime(frameTime), lastUpdate(SDL_GetTicks()),
      frameWidth(frameWidth), frameHeight(frameHeight) {

    SDL_Surface* surf = IMG_Load(spritesheet.c_str());
    if (!surf) {
        SDL_Log("Failed to load %s: %s", spritesheet.c_str(), IMG_GetError());
    }
    texture = SDL_CreateTextureFromSurface(renderer, surf);
    SDL_FreeSurface(surf);

    // frames theo chiều dọc
    for (int i = 0; i < frameCount; i++) {
        SDL_Rect clip = { 0, i * frameHeight, frameWidth, frameHeight };
        clips.push_back(clip);
    }
}

AnimatedSprite::~AnimatedSprite() {
    SDL_DestroyTexture(texture);
}

void AnimatedSprite::update(float deltaTime) {
    timer += deltaTime * 1000.0f; // deltaTime là giây → ms
    if (timer >= frameTime) {
        currentFrame = (currentFrame + 1) % clips.size();
        timer = 0.0f;
    }
}

void AnimatedSprite::render(SDL_Renderer* renderer, int x, int y, int scale) {
    SDL_Rect dst = { x, y, frameWidth * scale, frameHeight * scale };
    SDL_RenderCopy(renderer, texture, &clips[currentFrame], &dst);
}
