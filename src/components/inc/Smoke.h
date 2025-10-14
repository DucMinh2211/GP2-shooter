

#include <string>
#include "AnimatedSprite.h"
#include "Obstacle.h"
#include "math/Vector2.h"

class Smoke : public Obstacle {
public:
    Smoke(SDL_Renderer* renderer, const std::string& sheetPath, Vector2 pos, int frameW, int frameH, int frameCount, int frameTime, int columns = 1);
    ~Smoke();
    void update(float dt) override;
    void render(SDL_Renderer* renderer) override;
    void collide(ICollidable* object) override { (void)object; }
    bool is_finished() const { return finished; }
private:
    AnimatedSprite* anim;
    float elapsed;
    float totalDurationMs;
    bool finished;
};
