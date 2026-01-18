#include "Player.h"

Player::Player(Mesh* m) : GameObject(m, glm::vec3(0.0f, -17.5f, 0.0f), 5.0f) {
    rotation.y = 180.0f;
    targetRotationY = 180.0f;
}

float Player::lerpAngle(float current, float target, float speed, float dt) {
    float diff = target - current;
    while (diff > 180.0f) diff -= 360.0f;
    while (diff < -180.0f) diff += 360.0f;
    return current + diff * speed * dt;
}

void Player::update(float dt) {
    rotation.y = lerpAngle(rotation.y, targetRotationY, 10.0f, dt);
}