#pragma once
#include "HitBox.h"
#include "math/Vector2.h"
#include <SDL2/SDL.h>
#include <vector>
#include "Circle.h"

class OBB : public HitBox {
private:
    Vector2 _center;   // Tâm OBB
    Vector2 _halfSize; // Nửa kích thước (width/2, height/2)
    float _angle;      // Góc xoay (radian)

public:
    OBB(Vector2 center, Vector2 halfSize, float angle = 0.0f);

    void set_transform(const Vector2& center, float angle);

    // Lấy ra 4 đỉnh sau khi xoay
    std::vector<Vector2> get_corners() const;
    Vector2 get_center() const { return _center; }

    // Vẽ debug
    void debug_draw(SDL_Renderer* renderer, SDL_Color color) override;

    bool is_collide(HitBox& other) override;

    // SAT collision check OBB-OBB
    bool is_collide(OBB& other);

    bool is_collide(Circle& circle);
};
