#include "camera.h"

Camera::Camera()
{
    this->cameraPosition = glm::vec3(0.0f, 0.0f, 100.0f);
    this->cameraViewDirection = glm::vec3(0.0f, 0.0f, -1.0f);
    this->cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
    this->distanceFromPlayer = 30.0f; 
    this->angleAroundPlayer = 0.0f;
    this->pitch = 20.0f; 
}

Camera::Camera(glm::vec3 cameraPosition, glm::vec3 cameraViewDirection, glm::vec3 cameraUp)
{
    this->cameraPosition = cameraPosition;
    this->cameraViewDirection = cameraViewDirection;
    this->cameraUp = cameraUp;
    this->distanceFromPlayer = 30.0f;
    this->angleAroundPlayer = 0.0f;
    this->pitch = 20.0f;
}

Camera::~Camera()
{
}

void Camera::processMouseInput(float xoffset, float yoffset, bool constrainPitch)
{
    float sensitivity = 0.3f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    angleAroundPlayer -= xoffset; // - to rotate naturally

    pitch += yoffset;

    // don't flip over the player
    if (constrainPitch)
    {
        if (pitch > 89.0f) pitch = 89.0f;
        if (pitch < -10.0f) pitch = -10.0f; 
    }
}

void Camera::updateCameraPosition(glm::vec3 playerPosition)
{
    //offsets based on pitch and yaw
    float horizDistance = distanceFromPlayer * cos(glm::radians(pitch));
    float vertDistance = distanceFromPlayer * sin(glm::radians(pitch));

    float theta = glm::radians(angleAroundPlayer);
    float offsetX = horizDistance * sin(theta);
    float offsetZ = horizDistance * cos(theta);

    //position relative to player
    // y offset  to look at the cat's body
    cameraPosition.x = playerPosition.x - offsetX;
    cameraPosition.z = playerPosition.z - offsetZ;
    cameraPosition.y = playerPosition.y + vertDistance + 5.0f;

    // look at the player 
    glm::vec3 target = playerPosition + glm::vec3(0.0f, 5.0f, 0.0f);
    cameraViewDirection = glm::normalize(target - cameraPosition);

    // recalc right & up
    cameraRight = glm::normalize(glm::cross(cameraViewDirection, glm::vec3(0.0f, 1.0f, 0.0f)));
    cameraUp = glm::normalize(glm::cross(cameraRight, cameraViewDirection));
}

glm::mat4 Camera::getViewMatrix()
{
    return glm::lookAt(cameraPosition, cameraPosition + cameraViewDirection, cameraUp);
}

glm::vec3 Camera::getCameraPosition() { 
    return cameraPosition; 
}
glm::vec3 Camera::getCameraViewDirection() {
    return cameraViewDirection; 
}
glm::vec3 Camera::getCameraUp() { 
    return cameraUp; 
}