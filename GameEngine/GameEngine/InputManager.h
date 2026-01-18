#pragma once
#include <glew.h>
#include <glfw3.h>
#include "Camera\camera.h"

class Application;

class InputManager {
public:
    static bool firstMouse;
    static float lastX, lastY;
    static bool isCursorLocked;
    static bool wasComboDown;

    static void mouse_callback(GLFWwindow* window, double xpos, double ypos, Camera& camera);
    static void handleCursorLock(GLFWwindow* window);
    static void updateKeyboard(GLFWwindow* window, Application& app, float deltaTime);
};