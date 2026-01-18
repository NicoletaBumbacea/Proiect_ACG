#pragma once
#include "GameObject.h"

class Player : public GameObject {
public:
    float targetRotationY;
    const float MODEL_CORRECTION_ANGLE = -90.0f;
    Player(Mesh* m);
    void update(float dt);
    float lerpAngle(float current, float target, float speed, float dt);
};