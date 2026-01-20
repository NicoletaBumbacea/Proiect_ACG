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
    startTransitionPos(0.0f)
{
    doorZOffset = 3.8f;

    bearPos = glm::vec3(23.0f, -20.0f, -102.0f);
    showBearDialog = false;
    pressedT = false;

    penguinPos = glm::vec3(205.0f, -20.0f, -40.0f);

    // fishing init
    fishingState = FISHING_IDLE;
    fishingTimer = 0.0f;
    timeToBite = 0.0f;
    fishCaughtCount = 0;
    fishingMessage = "Press Left Click to Cast";

    // shop init
    money = 0;
    showShop = false;

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
    bobberShader = new Shader("Shaders/bobber_vertex.glsl", "Shaders/bobber_fragment.glsl");

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
    std::vector<Texture> t_penguin = { {loadBMP("Resources/Textures/penguin.bmp"), "texture_diffuse"} };


    // mesh loader
    MeshLoaderObj loader;
    sunMesh = new Mesh(loader.loadObj("Resources/Models/sphere.obj", t_orange));
    plane = new Mesh(loader.loadObj("Resources/Models/plane.obj", t_sand));
    bear = new Mesh(loader.loadObj("Resources/Models/bear.obj", t_bear));
    penguin = new Mesh(loader.loadObj("Resources/Models/penguin.obj", t_penguin));
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

    // rod line
    glGenVertexArrays(1, &lineVAO);
    glGenBuffers(1, &lineVBO);
    glBindVertexArray(lineVAO);
    glBindBuffer(GL_ARRAY_BUFFER, lineVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * 50, NULL, GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glBindVertexArray(0);
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

    // get cat in cabin
    player->position.y = getTerrainHeight(player->position);

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

    // fishing waiting
    if (isFishing && !isTransitioning) {
        if (fishingState == FISHING_WAITING) {
            fishingTimer += deltaTime;

            if (fishingTimer >= timeToBite) {
                fishingState = FISHING_BITING;
                fishingMessage = "!!! FISH ON !!! CLICK NOW !!!";
            }
        }
    }

    if (castAnimTimer > 0.0f) {
        castAnimTimer += deltaTime;
        if (castAnimTimer >= 0.6f) {
            castAnimTimer = 0.0f;
        }
    }
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
    

        // fishing
        if (isFishing) {
            ImGui::SetNextWindowPos(ImVec2(780, 780), ImGuiCond_Always);
            ImGui::Begin("Fishing Status", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_AlwaysAutoResize);
            ImGui::SetWindowFontScale(1.5f);

            if (fishingState == FISHING_BITING) {
                ImGui::TextColored(ImVec4(1, 0, 0, 1), "%s", fishingMessage.c_str());
            }
            else {
                ImGui::TextColored(ImVec4(1, 1, 1, 1), "%s", fishingMessage.c_str());
            }
            ImGui::Text("Fish Caught: %d", fishCaughtCount);
            ImGui::End();
        }
        else {
            ImGuiIO& io = ImGui::GetIO();
            float padding = 20.0f;

            ImGui::SetNextWindowPos(
                ImVec2(io.DisplaySize.x - padding, padding),
                ImGuiCond_Always,
                ImVec2(1.0f, 0.0f)
            );

            ImGui::SetNextWindowBgAlpha(0.0f);
            ImGui::Begin("RoamingHUD", nullptr,
                ImGuiWindowFlags_NoTitleBar |
                ImGuiWindowFlags_AlwaysAutoResize |
                ImGuiWindowFlags_NoMove |
                ImGuiWindowFlags_NoInputs |
                ImGuiWindowFlags_NoDecoration
            );

            ImGui::SetWindowFontScale(1.5f);
            ImGui::TextColored(ImVec4(0.0f, 0.0f, 0.0f, 1.0f), "Fish in Backpack: %d", fishCaughtCount);
            ImGui::TextColored(ImVec4(0.2f, 1.0f, 0.2f, 1.0f), "Cash: $%d", money);

            ImGui::End();
        }

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

    if (showShop) {
        ImGui::SetNextWindowPos(ImVec2(300, 300), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(ImVec2(400, 300), ImGuiCond_FirstUseEver);

        // Pass '&showShop' so the X button in the corner works to close it
        ImGui::Begin("Interaction Menu (Fish Shop)", &showShop);
        ImGui::SetWindowFontScale(1.2f);

        // --- TOP RIGHT STATS ---
        // 1. Save current cursor position (Left side)
        float startY = ImGui::GetCursorPosY();

        // 2. Calculate text width to align it to the right
        // We assume roughly 150 pixels for the text block
        ImGui::SameLine(ImGui::GetWindowWidth() - 180);

        // 3. Draw the stats
        ImGui::BeginGroup(); // Group them to keep them aligned
        ImGui::TextColored(ImVec4(1, 1, 0, 1), "Backpack: %d Fish", fishCaughtCount);
        ImGui::TextColored(ImVec4(0, 1, 0, 1), "Cash: $%d", money);
        ImGui::EndGroup();

        // 4. Reset cursor to next line on the left for the main content
        ImGui::SetCursorPosY(startY + 50.0f);
        ImGui::Separator();

        // --- SHOP CONTENT ---
        ImGui::Text("Welcome to the Trading Post!");
        ImGui::Text("Current Price: $%d / fish", FISH_PRICE);
        ImGui::Spacing();
        ImGui::Spacing();

        // Button 1: Sell One
        if (fishCaughtCount > 0) {
            if (ImGui::Button("Sell 1 Fish ($100)", ImVec2(160, 40))) {
                fishCaughtCount--;
                money += FISH_PRICE;
            }
        }
        else {
            // Greyed out button if no fish
            ImGui::BeginDisabled();
            ImGui::Button("No Fish to Sell", ImVec2(160, 40));
            ImGui::EndDisabled();
        }

        ImGui::SameLine(); // Put next button on the same row

        // Button 2: Sell All
        if (fishCaughtCount > 0) {
            if (ImGui::Button("Sell ALL", ImVec2(160, 40))) {
                int earnings = fishCaughtCount * FISH_PRICE;
                money += earnings;
                fishCaughtCount = 0;
            }
        }
        else {
            ImGui::BeginDisabled();
            ImGui::Button("Sell ALL", ImVec2(160, 40));
            ImGui::EndDisabled();
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

    glm::mat4 penguinM = glm::rotate(glm::scale(glm::translate(glm::mat4(1.0f), glm::vec3(205.0f, -20.0f, -40.0f)), glm::vec3(0.075f)), (0.0f), glm::vec3(0, 1, 0));
    glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &(proj* view* penguinM)[0][0]);
    glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &penguinM[0][0]);
    penguin->draw(*mainShader);

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
            rodM = glm::inverse(view);
            rodM = glm::translate(rodM, glm::vec3(1.5f, -1.5f, -2.5f));

            float castAngle = 0.0f;
            if (castAnimTimer > 0.0f) {
                if (castAnimTimer < 0.4f) {
                    float t = castAnimTimer / 0.4f;
                    castAngle = t * 50.0f;
                }
                else {
                    float t = (castAnimTimer - 0.4f) / 0.2f;
                    castAngle = 50.0f * (1.0f - t);
                }
            }
            rodM = glm::rotate(rodM, (-10.0f + castAngle), glm::vec3(1, 0, 0));
            rodM = glm::rotate(rodM, (170.0f), glm::vec3(0, 1, 0));
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

    // fishing line draw logic
    if (hasFishingRod && isFishing && !isTransitioning) {

        glm::vec3 rodTipPos = glm::vec3(rodM * glm::vec4(0.0f, 7.5f, 0.0f, 1.0f));

        std::vector<float> lineVertices;
        int segments = 20;

        if (fishingState == FISHING_IDLE) {
            glm::vec3 currentLineEnd = rodTipPos + glm::vec3(0.0f, -6.5f, 0.0f);

            float swing = sin(glfwGetTime() * 3.0f) * 0.1f;
            currentLineEnd.x += swing;

            lineVertices.push_back(rodTipPos.x); lineVertices.push_back(rodTipPos.y); lineVertices.push_back(rodTipPos.z);
            lineVertices.push_back(currentLineEnd.x); lineVertices.push_back(currentLineEnd.y); lineVertices.push_back(currentLineEnd.z);
        }
        else {
            float slack;
            if (fishingState == FISHING_BITING) {
                slack = (sin(glfwGetTime() * 50.0f) + 1.0f) * 0.2f;
            }
            else {
                slack = 10.0f;
            }

            glm::vec3 p0 = rodTipPos;
            glm::vec3 p2 = hookWorldPos;

            glm::vec3 p1 = (p0 + p2) * 0.5f;
            p1.y -= slack;

            for (int i = 0; i <= segments; i++) {
                float t = (float)i / (float)segments;

                float u = 1.0f - t;
                float tt = t * t;
                float uu = u * u;

                glm::vec3 p = (uu * p0) + (2 * u * t * p1) + (tt * p2);

                lineVertices.push_back(p.x);
                lineVertices.push_back(p.y);
                lineVertices.push_back(p.z);
            }
        }

        glBindVertexArray(lineVAO);
        glBindBuffer(GL_ARRAY_BUFFER, lineVBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, lineVertices.size() * sizeof(float), lineVertices.data());

        sunShader->use();
        glm::mat4 lineModel = glm::mat4(1.0f);
        glUniformMatrix4fv(glGetUniformLocation(sunShader->getId(), "MVP"), 1, GL_FALSE, &(proj * view * lineModel)[0][0]);

        glLineWidth(2.0f);

        if (fishingState == FISHING_IDLE) {
            glDrawArrays(GL_LINES, 0, 2);
        }
        else {
            glDrawArrays(GL_LINE_STRIP, 0, segments + 1);
        }

        glLineWidth(1.0f);
        glBindVertexArray(0);

        if (fishingState != FISHING_IDLE) {
            glm::vec3 bobberPos = hookWorldPos;
            if (fishingState == FISHING_BITING) {
                bobberPos.y += 0.5f;
                float jerk = sin(glfwGetTime() * 50.0f) * 0.15f;
                bobberPos.y += jerk;
            }
            else {
                bobberPos.y += sin(glfwGetTime() * 4.0f) * 0.1f;
            }

            bobberShader->use();

            glm::mat4 bobberModel = glm::translate(glm::mat4(1.0f), bobberPos);

            bobberModel = glm::rotate(bobberModel, glm::radians(15.0f), glm::vec3(0, 0, 1));
            bobberModel = glm::scale(bobberModel, glm::vec3(0.075f));

            glUniformMatrix4fv(glGetUniformLocation(bobberShader->getId(), "model"), 1, GL_FALSE, &bobberModel[0][0]);
            glUniformMatrix4fv(glGetUniformLocation(bobberShader->getId(), "view"), 1, GL_FALSE, &view[0][0]);
            glUniformMatrix4fv(glGetUniformLocation(bobberShader->getId(), "projection"), 1, GL_FALSE, &proj[0][0]);

            sunMesh->draw(*bobberShader);
        }
    }

    mainShader->use();

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
    glm::mat4 doorM = glm::rotate(glm::translate(cabM, glm::vec3(-0.6f, 0.0f, 1.35f + currentDoorSlide)), (45.0f), glm::vec3(0, 1, 0));
    doorM = glm::translate(doorM, glm::vec3(0.0f, 0.0f, doorZOffset));
    glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &(proj * view * doorM)[0][0]);
    cabinDoor->draw(*mainShader);

    // marker
    if (!showShop && !isFishing) {
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
            showShop = !showShop;

            if (showShop) showTaskWindow = false;
            else showTaskWindow = true;
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
    if (glm::distance(pos, glm::vec3(-120, -18, -85)) < 103) return true;
    if (glm::distance(pos, glm::vec3(200, -18, 120)) < 70) return true;
    return false;
}

float Application::getTerrainHeight(glm::vec3 pos) {
    groundLevel = -17.5f;
    cabinFloorLevel = -9.0f;

    if (pos.z > -76.0f && pos.z < -63.0f &&
        pos.x > 173.0f && pos.x < 180.0f) {

        float t = (pos.x - 173.0f) / (179.0f - 173.0f);

        if (t < 0.0f) t = 0.0f;
        if (t > 1.0f) t = 1.0f;

        return glm::mix(groundLevel, cabinFloorLevel, t);
    }

    if (pos.x > 179.0f && pos.x < 229.0f &&
        pos.z > -95.0f && pos.z < -47.0f) {
        return cabinFloorLevel;
    }

    return groundLevel;
}

bool Application::CheckCollision(glm::vec3 nextPos) {
    
   //preventing falling off the edge
    if (nextPos.x < -270.0f || nextPos.x > 270.0f ||
        nextPos.z < -230.0f || nextPos.z > 203.0f) {
        return true; 
    }

    // lake (rectangle AABB)
    float lakeMinX = 200.0f - 60.0f;
    float lakeMaxX = 200.0f + 60.0f;
    float lakeMinZ = 120.0f - 60.0f;
    float lakeMaxZ = 120.0f + 60.0f;

    if (nextPos.x > lakeMinX && nextPos.x < lakeMaxX &&
        nextPos.z > lakeMinZ && nextPos.z < lakeMaxZ) {
        return true;
    }

    // river (circle)
    float riverX = -120.0f;
    float riverZ = -85.0f;
    float riverRadius = 101.0f; 
    float distToRiver = glm::distance(glm::vec2(nextPos.x, nextPos.z), glm::vec2(riverX, riverZ));

    if (distToRiver < riverRadius) {
        return true;
    }

    struct Obstacle {
        glm::vec3 pos;
        float radius;
    };

    std::vector<Obstacle> obstacles = {
        // bear
        { glm::vec3(23.0f, -20.0f, -102.0f), 5.0f },
        // trees 
        { glm::vec3(225.0f, -20.0f, -162.0f), 4.0f },
        { glm::vec3(205.0f, -20.0f, -175.0f), 4.0f },
        { glm::vec3(170.0f, -20.0f, -155.0f), 4.0f },
        // penguin
        { glm::vec3(205.0f, -20.0f, -40.0f), 4.0f },
      
    };

    for (const auto& obs : obstacles) {
        float d = glm::distance(glm::vec2(nextPos.x, nextPos.z), glm::vec2(obs.pos.x, obs.pos.z));
        if (d < obs.radius) {
            return true;
        }
    }

    // boat
    float boatX = -28.0f;
    float boatZ = -56.0f;
    float boatWidth = 8.0f;  
    float boatLength = 18.0f; 

    if (nextPos.x > (boatX - boatWidth) && nextPos.x < (boatX + boatWidth) &&
        nextPos.z >(boatZ - boatLength) && nextPos.z < (boatZ + boatLength)) {
        return true;
    }

   // hammock
    float hamX = 210.0f;
    float hamZ = 25.0f;
    float hamWidth = 5.0f;
    float hamLength = 26.0f;

    if (nextPos.x > (hamX - hamWidth) && nextPos.x < (hamX + hamWidth) &&
        nextPos.z >(hamZ - hamLength) && nextPos.z < (hamZ + hamLength)) {
        return true;
    }
    //cabin
    // back wall
    if (nextPos.x > 225.0f && nextPos.x < 232.0f &&
        nextPos.z > -95.0f && nextPos.z < -45.0f) return true;

    // left wall
    if (nextPos.x > 178.0f && nextPos.x < 230.0f &&
        nextPos.z > -98.0f && nextPos.z < -92.0f) return true;

    // right wall
    if (nextPos.x > 178.0f && nextPos.x < 230.0f &&
        nextPos.z > -50.0f && nextPos.z < -44.0f) return true;

    // front wall (+door)
    float frontWallX_Min = 178.0f;
    float frontWallX_Max = 182.0f;

    // left of door
    if (nextPos.z > -95.0f && nextPos.z < -76.0f &&
        nextPos.x > frontWallX_Min && nextPos.x < frontWallX_Max) return true;

    // right of door
    if (nextPos.z > -63.0f && nextPos.z < -45.0f &&
        nextPos.x > frontWallX_Min && nextPos.x < frontWallX_Max) return true;

    return false;
}

void Application::handleMouseClick() {
    if (!isFishing || isTransitioning) return;

    if (fishingState == FISHING_IDLE) {
        glm::vec3 camPos = camera.getCameraPosition();
        glm::vec3 camDir = camera.getCameraViewDirection();

        hookWorldPos = camPos + glm::normalize(glm::vec3(camDir.x, 0, camDir.z)) * 35.0f;

        hookWorldPos.y = -20.0f;

        // Trigger Animation
        castAnimTimer = 0.0001f;

        fishingState = FISHING_WAITING;
        fishingTimer = 0.0f;
        timeToBite = 2.0f + (rand() % 20) / 10.0f;
        fishingMessage = "Waiting for a bite...";
        castAnimTimer = 0.0001f;
    }
    else if (fishingState == FISHING_WAITING) {
        fishingState = FISHING_IDLE;
        fishingMessage = "Pulled too early! Try again.";
    }
    else if (fishingState == FISHING_BITING) {
        fishingState = FISHING_IDLE;
        fishCaughtCount++;
        fishingMessage = "Caught a fish! Total: " + std::to_string(fishCaughtCount);
    }
}

Application::~Application() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}