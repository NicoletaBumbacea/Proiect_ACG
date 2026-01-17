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

	float rotationOx; // pitch up/down
	float rotationOy; //yaw left /right

public:
	Camera();
	Camera(glm::vec3 cameraPosition);
	Camera(glm::vec3 cameraPosition, glm::vec3 cameraViewDirection, glm::vec3 cameraUp);
	~Camera();

	glm::mat4 getViewMatrix();
	glm::vec3 getCameraPosition();
	glm::vec3 getCameraViewDirection();
	glm::vec3 getCameraUp();

	// setters
	void setCameraPosition(glm::vec3 pos);
	void setCameraViewDirection(glm::vec3 direction); //force look dir

	// movement
	void keyboardMoveFront(float cameraSpeed);
	void keyboardMoveBack(float cameraSpeed);
	void keyboardMoveLeft(float cameraSpeed);
	void keyboardMoveRight(float cameraSpeed);
	void keyboardMoveUp(float cameraSpeed);
	void keyboardMoveDown(float cameraSpeed);

	// mouse rotation
	void processMouseMovement(float xoffset, float yoffset); 

	float getPitch() { 
		return rotationOx; 
	}
	float getYaw() { 
		return rotationOy; 
	}
};