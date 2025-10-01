#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <vector>
#include <string>

class AnimatedSprite {
public:
    AnimatedSprite(SDL_Renderer* renderer, const std::string& spritesheet,
                   int frameWidth, int frameHeight, int frameCount, int frameTime);
    ~AnimatedSprite();

    void update(float deltaTime);
    void render(SDL_Renderer* renderer, int x, int y, int scale = 1, double angle = 0.0);
    SDL_Texture* get_texture() const { return texture; }


private:
    SDL_Texture* texture;
    std::vector<SDL_Rect> clips;
    int currentFrame;
    int frameTime;      // thời gian mỗi frame (ms)
    float timer;     
    Uint32 lastUpdate;
    int frameWidth, frameHeight;
};
