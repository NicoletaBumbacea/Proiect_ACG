#pragma once

#include <glm.hpp>
#include <gtx\transform.hpp>
#include <gtc\matrix_transform.hpp>
#include <gtc\type_ptr.hpp>
#include "..\Graphics\window.h"

class Camera
{
private:
    glm::vec3 cameraPosition;
    glm::vec3 cameraViewDirection;
    glm::vec3 cameraUp;
    glm::vec3 cameraRight;

    // Orbit parameters
    float distanceFromPlayer;
    float pitch;             // pitch 
    float angleAroundPlayer; //yaw 
   

public:
    Camera();
    Camera(glm::vec3 cameraPosition, glm::vec3 cameraViewDirection, glm::vec3 cameraUp);
    ~Camera();

    glm::mat4 getViewMatrix();
    glm::vec3 getCameraPosition();
    glm::vec3 getCameraViewDirection();
    glm::vec3 getCameraUp();

    void processMouseInput(float xoffset, float yoffset, bool constrainPitch = true);
    void updateCameraPosition(glm::vec3 playerPosition);

    // Auto centering logic
    float getYaw() const { 
        return angleAroundPlayer; 
    }
    void setYaw(float angle) { 
        angleAroundPlayer = angle; 
    }
    void setPitch(float angle) { 
        pitch = angle; 
    }
    void setCameraPosition(glm::vec3 newPos) { cameraPosition = newPos; }
};