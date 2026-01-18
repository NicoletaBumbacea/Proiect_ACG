#pragma once
#include <glew.h> 
#include <glfw3.h>
#include <vector>
#include <string>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include "Graphics/window.h"
#include "Camera\camera.h"
#include "Renderer.h"
#include "Player.h"
#include "Fish.h"
#include "SkyBox.h"
#include "Shaders/shader.h"
#include "Model Loading/mesh.h"
#include "Model Loading/texture.h"
#include "Model Loading/meshLoaderObj.h"

class InputManager;

class Application {
    friend class InputManager;
public:
    Application();
    ~Application();
    void run();

    void handleEnterPressed();
    void handleEPressed();

private:
    void initImGui();
    void initAssets();
    void update();
    void render();
    void updateTitle();
    bool checkNearWater(glm::vec3 pos);

    Window window;
    Camera camera;
    Renderer renderer;
    Skybox mySkybox;
    Player* player;
    GLFWwindow* extraWindow;

    std::vector<Fish*> schoolOfFish;
    std::vector<Fish*> schoolOfOceanFish;

    float deltaTime, lastFrame, titleUpdateTimer;
    bool hasFishingRod, isFishing, isTransitioning, isZoomingIn, isDoorOpen;
    bool showTaskWindow, eKeyPressedLastFrame, enterPressedLastFrame;
    float transitionTimer, transitionDuration, currentDoorSlide;

    glm::vec3 rodWorldPos, cabinPos, interactionPoint, lightPos, lightColor, startTransitionPos;

    Shader* mainShader, * sunShader, * waterShader, * riverShader, * skyboxShader;

    Mesh* sunMesh, * plane, * waterMesh, * riverMesh, * boat, * reed, * tree;
    Mesh* fishMesh, * hammock, * hammockTrees, * fishingRod, * cabin;
    Mesh* cabinFoundation, * cabinRoof, * cabinDoor, * frog;
};