#include "camera.h"

Camera::Camera(glm::vec3 cameraPosition)
{
	this->cameraPosition = cameraPosition;
	this->cameraViewDirection = glm::vec3(0.0f, 0.0f, -1.0f);
	this->cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
	this->cameraRight = glm::cross(cameraViewDirection, cameraUp);
	this->rotationOx = 0.0f;   // pitch
	this->rotationOy = -90.0f; // yaw 
}

Camera::Camera()
{
	this->cameraPosition = glm::vec3(0.0f, 0.0f, 100.0f);
	this->cameraViewDirection = glm::vec3(0.0f, 0.0f, -1.0f);
	this->cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
	this->cameraRight = glm::cross(cameraViewDirection, cameraUp);
	this->rotationOx = 0.0f;
	this->rotationOy = -90.0f;
}

Camera::Camera(glm::vec3 cameraPosition, glm::vec3 cameraViewDirection, glm::vec3 cameraUp)
{
	this->cameraPosition = cameraPosition;
	this->cameraViewDirection = cameraViewDirection;
	this->cameraUp = cameraUp;
	this->cameraRight = glm::cross(cameraViewDirection, cameraUp);
	this->rotationOx = 0.0f;
	this->rotationOy = -90.0f;
}

Camera::~Camera() {}

// movement func
void Camera::keyboardMoveFront(float cameraSpeed) {
	cameraPosition += cameraViewDirection * cameraSpeed;
}
void Camera::keyboardMoveBack(float cameraSpeed) {
	cameraPosition -= cameraViewDirection * cameraSpeed;
}
void Camera::keyboardMoveLeft(float cameraSpeed) {
	cameraPosition -= cameraRight * cameraSpeed;
}
void Camera::keyboardMoveRight(float cameraSpeed) {
	cameraPosition += cameraRight * cameraSpeed;
}
void Camera::keyboardMoveUp(float cameraSpeed) {
	cameraPosition += cameraUp * cameraSpeed;
}
void Camera::keyboardMoveDown(float cameraSpeed) {
	cameraPosition -= cameraUp * cameraSpeed;
}

//mouse logic
void Camera::processMouseMovement(float xoffset, float yoffset)
{
	float sensitivity = 0.1f;
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	rotationOy -= xoffset; //yaw 
	rotationOx += yoffset; // itch

	// prevent screen to flip
	if (rotationOx > 89.0f) rotationOx = 89.0f;
	if (rotationOx < -10.0f) rotationOx = -10.0f; 
}

void Camera::setCameraPosition(glm::vec3 pos)
{
	this->cameraPosition = pos;
}

void Camera::setCameraViewDirection(glm::vec3 direction)
{
	this->cameraViewDirection = glm::normalize(direction);
	this->cameraRight = glm::normalize(glm::cross(cameraViewDirection, glm::vec3(0.0f, 1.0f, 0.0f)));
	this->cameraUp = glm::normalize(glm::cross(cameraRight, cameraViewDirection));
}

glm::mat4 Camera::getViewMatrix()
{
	return glm::lookAt(cameraPosition, cameraPosition + cameraViewDirection, cameraUp);
}

glm::vec3 Camera::getCameraPosition() { return cameraPosition; }
glm::vec3 Camera::getCameraViewDirection() { return cameraViewDirection; }
glm::vec3 Camera::getCameraUp() { return cameraUp; }