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
    void handleTPressed();
    void handleMouseClick();

    bool CheckCollision(glm::vec3 nextPos);
    
private:
    //initialization routines
    void initImGui();
    void initAssets();
    void update();
    void render();
    void updateTitle();
    //helper functions 
    bool checkNearWater(glm::vec3 pos);
    float getTerrainHeight(glm::vec3 pos);

    Window window;
    Camera camera;
    Renderer renderer;
    Skybox mySkybox;
    Player* player;
    bool showShop;

    std::vector<Fish*> schoolOfFish;
    std::vector<Fish*> schoolOfOceanFish;

    float deltaTime, lastFrame, titleUpdateTimer;
    bool hasFishingRod, isFishing, isTransitioning, isZoomingIn, isDoorOpen;
    bool showTaskWindow, eKeyPressedLastFrame, enterPressedLastFrame;
    float transitionTimer, transitionDuration, currentDoorSlide;
    float doorZOffset;
    float groundLevel;
    float cabinFloorLevel;
    const int FISH_PRICE = 100;

    // fishing action variables
    enum FishingState {
        FISHING_IDLE,
        FISHING_WAITING, 
        FISHING_BITING,  
        FISHING_CAUGHT   
    };

    FishingState fishingState;
    float fishingTimer;
    float timeToBite;        
    int fishCaughtCount;
    int money;
    std::string fishingMessage; 

    // rod animation
    float castAnimTimer = 0.0f;

    // rod line
    unsigned int lineVAO, lineVBO;
    glm::vec3 hookWorldPos;

    // rod bobber
    Shader* bobberShader;

    //task 1
    int currentTask = 0; 
    bool pressedW = false;
    bool pressedA = false;
    bool pressedS = false;
    bool pressedD = false;
    std::string currentTaskText = "Task 1: Morning Stretches. Move (W, A, S, D) to wake up.";

	//task 2
    glm::vec3 bearPos;        
    bool showBearDialog;      
    bool pressedT;

    // task 5
    glm::vec3 penguinPos;

    //day/night
    float timeOfDay = 8.0f;
    float daySpeed = 0.0875f;
    glm::vec3 sunLightDir; 
    glm::vec3 sunLightColor; 
    glm::vec3 skyColor;
    bool hasSleptOnce = false;

    // gui on/off
    bool f3PressedLastFrame = false;

    // buy system
    int currentCatSkin = 0;
    const int SKIN_PRICE = 200;
    bool hasBoughtUpgrade = false;

    // bed position
    glm::vec3 hammockPos = glm::vec3(210.0f, -20.0f, 25.0f);

    glm::vec3 rodWorldPos, cabinPos, interactionPoint, lightPos, lightColor, startTransitionPos;

    Shader* mainShader, * sunShader, * waterShader, * riverShader, * skyboxShader;

    Mesh* sunMesh, * plane, * waterMesh, * riverMesh, * boat, * reed, * tree;
    Mesh* fishMesh, * hammock, * hammockTrees, * fishingRod, * cabin;
    Mesh* cabinFoundation, * cabinRoof, * cabinDoor, * bear, * penguin;
};