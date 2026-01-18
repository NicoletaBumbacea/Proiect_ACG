#pragma once
#include "GameObject.h"

class Fish : public GameObject {
public:
    glm::vec3 target;
    float speed;
    bool isOcean;

    Fish(Mesh* m, bool oceanFish);
    void update(float dt);
    glm::vec3 getRandomPoint();
};