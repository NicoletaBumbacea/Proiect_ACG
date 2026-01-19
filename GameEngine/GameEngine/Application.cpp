#include <algorithm>
#include <iomanip>
#include <iostream>
#include <sstream>

#include "Application.h"
#include "InputManager.h"
#include "Model Loading/texture.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

Application::Application()
    : window("Game Engine", 800, 800),
    deltaTime(0.0f),
    lastFrame(0.0f),
    titleUpdateTimer(0.0f),
    hasFishingRod(false),
    isFishing(false),
    isTransitioning(false),
    isZoomingIn(false),
    isDoorOpen(false),
    showTaskWindow(true),
    eKeyPressedLastFrame(false),
    enterPressedLastFrame(false),
    transitionTimer(0.0f),
    transitionDuration(1.0f),
    currentDoorSlide(0.0f),
    rodWorldPos(190.0f, -20.0f, -195.0f),
    cabinPos(205.0f, -20.0f, -70.0f),
    interactionPoint(205.0f, -15.0f, -70.0f),
    lightPos(-180.0f, 100.0f, -200.0f),
    lightColor(1.0f, 1.0f, 1.0f),
    extraWindow(nullptr),
    startTransitionPos(0.0f)
{
   
    bearPos = glm::vec3(23.0f, -20.0f, -102.0f);
    showBearDialog = false;
    pressedT = false;

    //mouse callback to the camera with window pointer
    glfwSetWindowUserPointer(window.getWindow(), &camera);
    glfwSetCursorPosCallback(window.getWindow(),
        [](GLFWwindow* w, double x, double y) {
            Camera* cam = (Camera*)glfwGetWindowUserPointer(w);
            InputManager::mouse_callback(w, x, y, *cam);
        });

    glfwSetInputMode(window.getWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    initImGui();
    initAssets();
}

void Application::initImGui() {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    (void)io;
    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowRounding = 10.0f;
    style.FrameRounding = 5.0f;
    style.PopupRounding = 5.0f;
    style.ScrollbarRounding = 10.0f;
    style.GrabRounding = 5.0f;

    // GUI screen colors
    ImVec4* colors = style.Colors;
    colors[ImGuiCol_WindowBg] = ImVec4(0.1f, 0.1f, 0.15f, 0.85f);
    colors[ImGuiCol_TitleBg] = ImVec4(0.2f, 0.2f, 0.4f, 1.0f);
    colors[ImGuiCol_TitleBgActive] = ImVec4(0.3f, 0.3f, 0.6f, 1.0f);
    colors[ImGuiCol_Button] = ImVec4(0.2f, 0.5f, 0.2f, 1.0f);
    colors[ImGuiCol_ButtonHovered] = ImVec4(0.3f, 0.7f, 0.3f, 1.0f);
    colors[ImGuiCol_ButtonActive] = ImVec4(0.1f, 0.4f, 0.1f, 1.0f);
    colors[ImGuiCol_Text] = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);

    ImGui_ImplGlfw_InitForOpenGL(window.getWindow(), true);
    ImGui_ImplOpenGL3_Init("#version 330");
}

void Application::initAssets() {
    mainShader = new Shader("Shaders/vertex_shader.glsl", "Shaders/fragment_shader.glsl");
    sunShader = new Shader("Shaders/sun_vertex_shader.glsl", "Shaders/sun_fragment_shader.glsl");
    waterShader = new Shader("Shaders/water_vertex_shader.glsl", "Shaders/water_fragment_shader.glsl");
    riverShader = new Shader("Shaders/river_vertex_shader.glsl", "Shaders/river_fragment_shader.glsl");
    skyboxShader = new Shader("Shaders/skybox_vertex.glsl", "Shaders/skybox_fragment.glsl");

    // skybox
    mySkybox.setup();
    std::vector<std::string> faces = {
        "Resources/Textures/Skybox/px.png", "Resources/Textures/Skybox/nx.png",
        "Resources/Textures/Skybox/py.png", "Resources/Textures/Skybox/ny.png",
        "Resources/Textures/Skybox/nz.png", "Resources/Textures/Skybox/pz.png" };
    mySkybox.textureID = mySkybox.loadCubemap(faces);

    // textures
    std::vector<Texture> t_wood = { {loadBMP("Resources/Textures/wood.bmp"), "texture_diffuse"} };
    std::vector<Texture> t_water = { {loadBMP("Resources/Textures/watah.bmp"), "texture_diffuse"} };
    std::vector<Texture> t_orange = { {loadBMP("Resources/Textures/orange.bmp"), "texture_diffuse"} };
    std::vector<Texture> t_cat = { {loadBMP("Resources/Textures/cat.bmp"), "texture_diffuse"} };
    std::vector<Texture> t_sand = { {loadBMP("Resources/Textures/sand.bmp"), "texture_diffuse"} };
    std::vector<Texture> t_boat = { {loadBMP("Resources/Textures/boat_color.bmp"), "texture_diffuse"} };
    std::vector<Texture> t_reed = { {loadBMP("Resources/Textures/reed.bmp"), "texture_diffuse"} };
    std::vector<Texture> t_tree = { {loadBMP("Resources/Textures/forrest.bmp"), "texture_diffuse"} };
    std::vector<Texture> t_fish = { {loadBMP("Resources/Textures/fih.bmp"), "texture_diffuse"} };
    std::vector<Texture> t_hammock = { {loadBMP("Resources/Textures/hammock_tex.bmp"), "texture_diffuse"} };
    std::vector<Texture> t_hamWood = { {loadBMP("Resources/Textures/hammockTree_tex.bmp"), "texture_diffuse"} };
    std::vector<Texture> t_rod = { {loadBMP("Resources/Textures/fishingRod.bmp"), "texture_diffuse"} };
    std::vector<Texture> t_bright = { {loadBMP("Resources/Textures/brightwood.bmp"), "texture_diffuse"} };
    std::vector<Texture> t_dark = { {loadBMP("Resources/Textures/darkwood.bmp"), "texture_diffuse"} };
    std::vector<Texture> t_rock = { {loadBMP("Resources/Textures/rock_cabin.bmp"), "texture_diffuse"} };
    std::vector<Texture> t_bear = { {loadBMP("Resources/Textures/bear.bmp"), "texture_diffuse"} };

    // mesh loader
    MeshLoaderObj loader;
    sunMesh = new Mesh(loader.loadObj("Resources/Models/sphere.obj", t_orange));
    plane = new Mesh(loader.loadObj("Resources/Models/plane.obj", t_sand));
    bear = new Mesh(loader.loadObj("Resources/Models/bear.obj", t_bear));
    boat = new Mesh(loader.loadObj("Resources/Models/boat.obj", t_boat));
    reed = new Mesh(loader.loadObj("Resources/Models/reed.obj", t_reed));
    tree = new Mesh(loader.loadObj("Resources/Models/bigtree.obj", t_tree));
    fishMesh = new Mesh(loader.loadObj("Resources/Models/fih.obj", t_fish));
    hammock = new Mesh(loader.loadObj("Resources/Models/hammond.obj", t_hammock));
    hammockTrees = new Mesh(loader.loadObj("Resources/Models/cocotierul_vincent.obj", t_hamWood));
    fishingRod = new Mesh(loader.loadObj("Resources/Models/fishingRod.obj", t_rod));
    cabin = new Mesh(loader.loadObj("Resources/Models/cabin.obj", t_bright));
    cabinFoundation = new Mesh(loader.loadObj("Resources/Models/foundation.obj", t_rock));
    cabinRoof = new Mesh(loader.loadObj("Resources/Models/roof.obj", t_dark));
    cabinDoor = new Mesh(loader.loadObj("Resources/Models/door.obj", t_bright));

    // player
    player = new Player(new Mesh(loader.loadObj("Resources/Models/cat.obj", t_cat)));

    // procedural grids
    waterMesh = new Mesh(renderer.generateWaterGrid(120, 1.0f, t_water));
    riverMesh = new Mesh(renderer.generateCircularRiver(50.0f, 100.0f, 100, t_water));

    // fih school
    for (int i = 0; i < 5; i++) schoolOfFish.push_back(new Fish(fishMesh, false));
    for (int i = 0; i < 10; i++) schoolOfOceanFish.push_back(new Fish(fishMesh, true));
}

void Application::run() {
    while (!glfwWindowShouldClose(window.getWindow())) {
        float currentFrame = (float)glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        InputManager::handleCursorLock(window.getWindow());
        InputManager::updateKeyboard(window.getWindow(), *this, deltaTime);

        update();
        render();
    }
}

void Application::update() {
    // update obj
    player->update(deltaTime);

    if (currentTask == 0) {
        if (pressedW && pressedA && pressedS && pressedD) {
            currentTask = 1;
            currentTaskText = "Task 2: The Bear knows secrets. Talk to him at [23, -102] (Press T).";
            std::cout << "Task 1 Complete!" << std::endl;
        }
    }
    for (auto f : schoolOfFish) f->update(deltaTime);
    for (auto f : schoolOfOceanFish) f->update(deltaTime);

    //camera orbit logic
    camera.updateCameraPosition(player->position);

    glm::vec3 thirdPersonPos = camera.getCameraPosition();
    glm::vec3 firstPersonPos = player->position + glm::vec3(0.0f, 8.0f, 0.0f);

    // transition handling
    if (isTransitioning) {
        transitionTimer += deltaTime;
        float t = std::min(transitionTimer / transitionDuration, 1.0f);
        t = t * t * (3.0f - 2.0f * t);  // zooming in

        if (transitionTimer >= transitionDuration) {
            isTransitioning = false;
            isFishing = isZoomingIn;
            camera.setCameraPosition(isFishing ? firstPersonPos : thirdPersonPos);
        }
        else {
            glm::vec3 target = isZoomingIn ? firstPersonPos : thirdPersonPos;
            camera.setCameraPosition(glm::mix(startTransitionPos, target, t));
        }
    }
    else if (isFishing) {
        camera.setCameraPosition(firstPersonPos);
    }

    // door animation logic
    float targetSlide = isDoorOpen ? 1.2f : 0.0f;
    float slideSpeed = 2.0f * deltaTime;
    if (currentDoorSlide < targetSlide)
        currentDoorSlide = std::min(currentDoorSlide + slideSpeed, targetSlide);
    else if (currentDoorSlide > targetSlide)
        currentDoorSlide = std::max(currentDoorSlide - slideSpeed, targetSlide);

    updateTitle();
}

void Application::render() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

   
    if (showTaskWindow) {
        ImGui::SetNextWindowPos(ImVec2(20, 20), ImGuiCond_Always);
        ImGui::SetNextWindowSize(ImVec2(500, 350), ImGuiCond_Always);
        ImGui::Begin("Quest Log", nullptr, ImGuiWindowFlags_NoCollapse);
        ImGui::SetWindowFontScale(1.5f);
        ImGui::Text("Agamitza goes fishing");
        ImGui::Separator();
        ImGui::TextColored(ImVec4(1, 1, 0, 1), "Current Task:");
        ImGui::TextWrapped("%s", currentTaskText.c_str());

        if (currentTask == 0) {
            ImGui::Spacing();
            ImGui::Text("Stretches:");
            ImGui::BulletText(pressedW ? "Forward [X]" : "Forward [ ]");
            ImGui::BulletText(pressedA ? "Left    [X]" : "Left    [ ]");
            ImGui::BulletText(pressedS ? "Back    [X]" : "Back    [ ]");
            ImGui::BulletText(pressedD ? "Right   [X]" : "Right   [ ]");
        }

        ImGui::Spacing();
        ImGui::Separator();
        if (ImGui::Button("Close Menu")) showTaskWindow = false;
        ImGui::End();
    }

    if (showBearDialog) {
        ImGuiIO& io = ImGui::GetIO();
        ImVec2 center(io.DisplaySize.x * 0.5f, io.DisplaySize.y * 0.5f);
        ImGui::SetNextWindowPos(center, ImGuiCond_Always, ImVec2(0.5f, 0.5f));
        ImGui::SetNextWindowSize(ImVec2(400, 250), ImGuiCond_Always);
        ImGui::SetWindowFontScale(1.4f);

        ImGui::Begin("Interaction", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize);
        ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.0f, 1.0f), "Wise Bear says:");
        ImGui::Separator();
        ImGui::TextWrapped("Grrrr... hello little cat.");
        ImGui::Spacing();
        ImGui::TextWrapped("You look hungry. I saw a Fishing Rod left by the humans near the river bank.");
        ImGui::Spacing();
        ImGui::TextWrapped("Go to coordinates: [190, -195]");
        ImGui::Separator();
        if (ImGui::Button("Thank you, Mr. Bear!", ImVec2(380, 40))) {
            showBearDialog = false;
            currentTask = 2;
            currentTaskText = "Task 3: Find the Rod at [190, -195] and pick it up (Press E).";
        }
        ImGui::End();
    }

    window.clear();

    //shared matrices
    float aspect = (float)window.getWidth() / (float)window.getHeight();
    glm::mat4 proj = glm::perspective(90.0f, aspect, 0.1f, 10000.0f);
    glm::mat4 view = camera.getViewMatrix();
    glm::vec3 camPos = camera.getCameraPosition();

    //draw skybox
    mySkybox.draw(*skyboxShader, view, proj);

    // draw water & river
    waterShader->use();
    renderer.applyLighting(*waterShader, lightPos, lightColor, camPos);
    glm::mat4 waterModel = glm::translate(glm::mat4(1.0f), glm::vec3(200.0f, -20.0f, 120.0f));
    glUniformMatrix4fv(glGetUniformLocation(waterShader->getId(), "MVP"), 1, GL_FALSE, &(proj * view * waterModel)[0][0]);
    glUniform1f(glGetUniformLocation(waterShader->getId(), "time"), (float)glfwGetTime());
    waterMesh->draw(*waterShader);

    riverShader->use();
    renderer.applyLighting(*riverShader, lightPos, lightColor, camPos);
    glm::mat4 riverModel = glm::translate(glm::mat4(1.0f), glm::vec3(-120.0f, -19.5f, -85.0f));
    glUniformMatrix4fv(glGetUniformLocation(riverShader->getId(), "MVP"), 1, GL_FALSE, &(proj * view * riverModel)[0][0]);
    glUniform1f(glGetUniformLocation(riverShader->getId(), "time"), (float)glfwGetTime());
    riverMesh->draw(*riverShader);

    //main shader for models
    mainShader->use();
    renderer.applyLighting(*mainShader, lightPos, lightColor, camPos);
    GLuint MatrixID = glGetUniformLocation(mainShader->getId(), "MVP");
    GLuint ModelMatrixID = glGetUniformLocation(mainShader->getId(), "model");

    //static environment
    glm::mat4 pM = glm::scale(glm::translate(glm::mat4(1.0f), glm::vec3(0, -20, 0)), glm::vec3(3.0f));
    glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &(proj * view * pM)[0][0]);
    glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &pM[0][0]);
    plane->draw(*mainShader);


    glm::mat4 bearM = glm::rotate(glm::scale(glm::translate(glm::mat4(1.0f), glm::vec3(23.0f, -20, -102)), glm::vec3(1.5f)), (45.0f), glm::vec3(0, 1, 0));
    glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &(proj * view * bearM)[0][0]);
    glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &bearM[0][0]);
    bear->draw(*mainShader);

    glm::mat4 boatM = glm::scale(glm::translate(glm::mat4(1.0f), glm::vec3(-28, -20, -56)), glm::vec3(0.1f));
    glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &(proj * view * boatM)[0][0]);
    boat->draw(*mainShader);

    // reeds & trees
    std::vector<glm::vec3> reeds = { glm::vec3(-25, -20, -80),
                                     glm::vec3(-30, -20, -75),
                                     glm::vec3(-20, -20, -82) };
    for (auto& pos : reeds) {
        glm::mat4 m = glm::scale(glm::translate(glm::mat4(1.0f), pos), glm::vec3(2.0f));
        glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &(proj * view * m)[0][0]);
        reed->draw(*mainShader);
    }
    std::vector<glm::vec3> trees = { glm::vec3(225, -20, -162),
                                     glm::vec3(205, -20, -175),
                                     glm::vec3(170, -20, -155) };
    for (auto& pos : trees) {
        glm::mat4 m = glm::scale(glm::translate(glm::mat4(1.0f), pos), glm::vec3(3.0f));
        glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &(proj * view * m)[0][0]);
        tree->draw(*mainShader);
    }

    // fih
    for (auto f : schoolOfFish) f->draw(*mainShader, view, proj);
    for (auto f : schoolOfOceanFish) f->draw(*mainShader, view, proj);

    // fishing rod transformation
    glm::mat4 rodM(1.0f);
    if (hasFishingRod) {
        if (isFishing && !isTransitioning) {
            // first person
            rodM =
                glm::inverse(view) *
                glm::translate(glm::mat4(1.0), glm::vec3(1.5f, -1.5f, -2.5f)) *
                glm::rotate(glm::mat4(1.0), glm::radians(-10.0f),
                    glm::vec3(1, 0, 0)) *
                glm::rotate(glm::mat4(1.0), glm::radians(170.0f), glm::vec3(0, 1, 0));
        }
        else {
            // third person back
            rodM = glm::translate(glm::mat4(1.0), player->position) *
                glm::rotate(glm::mat4(1.0), glm::radians(player->rotation.y),
                    glm::vec3(0.0f, 1.0f, 0.0f)) *
                glm::translate(glm::mat4(1.0), glm::vec3(1.0f, 10.0f, 1.5f)) *
                glm::translate(glm::mat4(1.0), glm::vec3(0.0f, -10.0f, -5.0f));
        }
    }
    else {
        // move to world position
        rodM = glm::translate(glm::mat4(1.0f), rodWorldPos);

        // spin 
        float spinSpeed = 50.0f;
        float spinAngle = (float)glfwGetTime() * spinSpeed;
        rodM = glm::rotate(rodM, spinAngle, glm::vec3(0.0f, 1.0f, 0.0f));

        // floating height
        float floatHeight = sin((float)glfwGetTime() * 2.0f) * 1.0f;
        rodM = glm::translate(rodM, glm::vec3(0.0f, floatHeight + 3.0f, 0.0f));

        // 45 degree tilt
        rodM = glm::rotate(rodM, 45.0f, glm::vec3(0.0f, 0.0f, 1.0f));
    }

    //scaling and draw call
    rodM = glm::scale(rodM, glm::vec3(1.5f, 1.5f, 1.5f));
    glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &(proj * view * rodM)[0][0]);
    glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &rodM[0][0]);
    fishingRod->draw(*mainShader);

    // hammock & cabin
    glm::mat4 hamBase = glm::rotate(glm::scale(glm::translate(glm::mat4(1.0f), glm::vec3(210, -20, 25)), glm::vec3(5.0f)), glm::radians(45.0f), glm::vec3(0, 1, 0));
    glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &(proj * view * hamBase)[0][0]);
    hammock->draw(*mainShader);
    glm::mat4 hamTr = glm::translate(hamBase, glm::vec3(0.4f, 0.0f, -4.5f));
    glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &(proj * view * hamTr)[0][0]);
    hammockTrees->draw(*mainShader);

    glm::mat4 cabM = glm::rotate(glm::scale(glm::translate(glm::mat4(1.0f), cabinPos), glm::vec3(4.0f)), 180.0f, glm::vec3(0, 1, 0));
    glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &(proj * view * cabM)[0][0]);
    cabinFoundation->draw(*mainShader);
    cabin->draw(*mainShader);
    cabinRoof->draw(*mainShader);
    glm::mat4 doorM = glm::translate(cabM, glm::vec3(-0.6f, 0.0f, 1.35f + currentDoorSlide));
    glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &(proj * view * doorM)[0][0]);
    cabinDoor->draw(*mainShader);

    // marker
    if (extraWindow == nullptr) {
        glm::mat4 markM = glm::translate(glm::mat4(1.0f), interactionPoint);
        glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &(proj * view * markM)[0][0]);
        sunMesh->draw(*mainShader);
    }

    // player visibility logic->hide during zoom
    bool drawCat = true;
    if (isFishing && !isTransitioning) drawCat = false;
    if (isTransitioning && isZoomingIn && (transitionTimer / transitionDuration > 0.8f))
        drawCat = false;
    if (drawCat) player->draw(*mainShader, view, proj);

    // light source
    sunShader->use();
    glm::mat4 sMat = glm::translate(glm::mat4(1.0f), lightPos);
    glUniformMatrix4fv(glGetUniformLocation(sunShader->getId(), "MVP"), 1, GL_FALSE, &(proj * view * sMat)[0][0]);
    sunMesh->draw(*sunShader);

    // popups
    if (extraWindow != nullptr) {
        if (glfwWindowShouldClose(extraWindow)) {
            glfwDestroyWindow(extraWindow);
            extraWindow = nullptr;
        }
        else {
            glfwMakeContextCurrent(extraWindow);
            glViewport(0, 0, 400, 300);
            glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glfwSwapBuffers(extraWindow);
            glfwMakeContextCurrent(window.getWindow());
        }
    }

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    window.update();
}

void Application::handleEnterPressed() {
    if (!isFishing && !isTransitioning && hasFishingRod &&
        checkNearWater(player->position)) {
        isTransitioning = true;
        isZoomingIn = true;
        transitionTimer = 0.0f;
        startTransitionPos = camera.getCameraPosition();
        InputManager::isCursorLocked = true;
        glfwSetInputMode(window.getWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    }
}

void Application::handleEPressed() {
    if (isFishing && !isTransitioning) {
        isTransitioning = true;
        isZoomingIn = false;
        transitionTimer = 0.0f;
        startTransitionPos = camera.getCameraPosition();
    }
    else {
        float distToRod = glm::length(player->position - rodWorldPos);
        if (hasFishingRod || distToRod < 50.0f) {
            if (!hasFishingRod)
                hasFishingRod = true;
            else {
                hasFishingRod = false;
                float yawRad = glm::radians(player->rotation.y);
                rodWorldPos =
                    player->position - (glm::vec3(sin(yawRad), 0, cos(yawRad)) * 10.0f);
                rodWorldPos.y = -20.0f;
            }
        }
        else if (glm::length(player->position - interactionPoint) < 15.0f) {
            if (extraWindow == nullptr)
                extraWindow = glfwCreateWindow(400, 300, "Interaction Menu", NULL,
                    window.getWindow());
        }
        else if (glm::length(player->position - cabinPos) < 40.0f)
            isDoorOpen = !isDoorOpen;
    }
}

void Application::updateTitle() {
    titleUpdateTimer += deltaTime;
    if (titleUpdateTimer > 0.1f) {
        std::stringstream ss;
        ss << std::fixed << std::setprecision(1);
        ss << "Pos: " << (int)player->position.x << "," << (int)player->position.z
            << " | Rod: " << (hasFishingRod ? "YES" : "NO");
        glfwSetWindowTitle(window.getWindow(), ss.str().c_str());
        titleUpdateTimer = 0.0f;
    }
}

void Application::handleTPressed() {

    if (currentTask == 1) {
        float distance = glm::distance(player->position, bearPos);
        if (distance < 25.0f) {
            showBearDialog = true; 
        }
    }
}

bool Application::checkNearWater(glm::vec3 pos) {
    if (glm::distance(pos, glm::vec3(-120, -18, -85)) < 95) return true;
    if (glm::distance(pos, glm::vec3(200, -18, 120)) < 70) return true;
    return false;
}

Application::~Application() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}