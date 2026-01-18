#pragma once
#include <glm.hpp>
#include <gtx/transform.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include "..\Graphics\window.h"

class Camera
{
private:
    glm::vec3 cameraPosition;
    glm::vec3 cameraViewDirection;
    glm::vec3 cameraUp;
    glm::vec3 cameraRight;

    // orbit parameters 
    float distanceFromPlayer;
    float pitch;             // vertical angle
    float angleAroundPlayer; // horizontal angle -yaw

public:
    Camera();
    Camera(glm::vec3 cameraPosition, glm::vec3 cameraViewDirection, glm::vec3 cameraUp);
    ~Camera();

    glm::mat4 getViewMatrix();
    glm::vec3 getCameraPosition();
    glm::vec3 getCameraViewDirection();
    glm::vec3 getCameraUp();

    // mouse logic for rotation
    void processMouseInput(float xoffset, float yoffset, bool constrainPitch = true);

    //orbit math logic
    void updateCameraPosition(glm::vec3 playerPosition);

    float getYaw() const { 
        return angleAroundPlayer; 
    }
    void setYaw(float angle) {
        angleAroundPlayer = angle;
    }
    void setPitch(float angle) {
        pitch = angle;
    }
    void setCameraPosition(glm::vec3 newPos) { 
        cameraPosition = newPos; 
    }
};