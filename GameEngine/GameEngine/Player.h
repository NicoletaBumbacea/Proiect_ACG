#pragma once
#include "GameObject.h"

class Player : public GameObject {
public:
    float targetRotationY;
    //obj faces right (+x) -> rotate -> froward (-z) 
    const float MODEL_CORRECTION_ANGLE = -90.0f;
    Player(Mesh* m);
    void update(float dt);
    //calc the shortest path between two angles
    float lerpAngle(float current, float target, float speed, float dt);
};