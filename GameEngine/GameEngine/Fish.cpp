#include "Fish.h"

Fish::Fish(Mesh* m, bool oceanFish) : GameObject(m), isOcean(oceanFish) {
    position = getRandomPoint();
    target = getRandomPoint();
    speed = isOcean ? (8.0f + (rand() % 5)) : (10.0f + (rand() % 10));
    scale = 3.0f;
}

glm::vec3 Fish::getRandomPoint() {
    if (isOcean) {
        glm::vec3 center(200.0f, -18.60f, 120.0f);
        float minRadius = 10.0f, maxRadius = 65.0f;
        float angle = (rand() % 360) * (3.14159f / 180.0f);
        float radius = minRadius + (rand() % (int)(maxRadius - minRadius));
        return glm::vec3(center.x + cos(angle) * radius, center.y - 2.0f, center.z + sin(angle) * radius);
    }
    else {
        glm::vec3 center(-120.0f, -18.6f, -85.0f);
        float minRadius = 15.0f, maxRadius = 90.0f;
        float angle = (rand() % 360) * (3.14159f / 180.0f);
        float radius = minRadius + (rand() % (int)(maxRadius - minRadius));
        return glm::vec3(center.x + cos(angle) * radius, center.y - 2.0f, center.z + sin(angle) * radius);
    }
}

void Fish::update(float dt) {
    glm::vec3 direction = target - position;
    if (glm::length(direction) < 1.0f) {
        target = getRandomPoint();
    }
    else {
        position += glm::normalize(direction) * speed * dt;
        rotation.y = glm::degrees(atan2(direction.x, direction.z)) + 90.0f;
    }
}