#include "InputManager.h"
#include "Application.h"

bool InputManager::firstMouse = true;
float InputManager::lastX = 400.0f;
float InputManager::lastY = 400.0f;
bool InputManager::isCursorLocked = true;
bool InputManager::wasComboDown = false;

void InputManager::mouse_callback(GLFWwindow* window, double xpos, double ypos, Camera& camera) {
    if (!isCursorLocked) return;
    if (firstMouse) {
        lastX = (float)xpos; lastY = (float)ypos;
        firstMouse = false;
    }
    float xoffset = (float)xpos - lastX;
    float yoffset = lastY - (float)ypos;
    lastX = (float)xpos; lastY = (float)ypos;
    camera.processMouseInput(xoffset, yoffset);
}

void InputManager::handleCursorLock(GLFWwindow* window) {
    bool ctrlDown = glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS;
    bool rightClick = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS;
    bool comboNow = ctrlDown && rightClick;

    if (comboNow && !wasComboDown) {
        isCursorLocked = !isCursorLocked;
        glfwSetInputMode(window, GLFW_CURSOR, isCursorLocked ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
        if (isCursorLocked) firstMouse = true;
    }
    wasComboDown = comboNow;
}

void InputManager::updateKeyboard(GLFWwindow* window, Application& app, float deltaTime) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) glfwSetWindowShouldClose(window, true);

    if (!app.isFishing) {
        float speed = 20.0f * deltaTime;
        glm::vec3 dir(0.0f);
        glm::vec3 fwd = app.camera.getCameraViewDirection();
        fwd.y = 0.0f; // lock to ground
        if (glm::length(fwd) > 0.001f) fwd = glm::normalize(fwd);
        glm::vec3 side = glm::normalize(glm::cross(fwd, glm::vec3(0, 1, 0)));

        bool moved = false;
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
            dir += fwd; moved = true;
            app.pressedW = true; //mark W as pressed
        }
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
            dir -= fwd; moved = true;
            app.pressedS = true; // mark S as pressed
        }
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
            dir -= side; moved = true;
            app.pressedA = true; // mark A as pressed
        }
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
            dir += side; moved = true;
            app.pressedD = true; // mark D as pressed
        }


        if (moved && glm::length(dir) > 0.01f) {
            dir = glm::normalize(dir);
            app.player->position += dir * speed;
            // circle logic for cat
            float angle = glm::degrees(atan2(dir.x, dir.z));
            app.player->targetRotationY = angle + app.player->MODEL_CORRECTION_ANGLE;
        }
    }

    static bool tWasDown = false;
    bool tIsDown = glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS;
    if (tIsDown && !tWasDown) {
        app.handleTPressed(); // Call the new function
    }
    tWasDown = tIsDown;

    // enter logic - fishing 
    static bool enterWasDown = false;
    bool enterIsDown = glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS;
    if (enterIsDown && !enterWasDown) app.handleEnterPressed();
    enterWasDown = enterIsDown;

    // e logic -interact
    static bool eWasDown = false;
    bool eIsDown = glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS;
    if (eIsDown && !eWasDown) app.handleEPressed();
    eWasDown = eIsDown;
}