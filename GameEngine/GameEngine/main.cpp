#include "Graphics\window.h"
#include "Camera\camera.h"
#include "Shaders\shader.h"
#include "Model Loading\mesh.h"
#include "Model Loading\texture.h"
#include "Model Loading\meshLoaderObj.h"
#include <iostream> 
#include <string>
#include <sstream> 
#include <iomanip> 
#include <vector>

void processKeyboardInput();
Mesh generateWaterGrid(int size, float spacing, std::vector<Texture> tex); 

float deltaTime = 0.0f;
float lastFrame = 0.0f;

Window window("Game Engine", 800, 800);
Camera camera;

glm::vec3 lightColor = glm::vec3(1.0f);
glm::vec3 lightPos = glm::vec3(-180.0f, 100.0f, -200.0f);

int main()
{
    glClearColor(0.2f, 0.8f, 1.0f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Load Shaders
    Shader shader("Shaders/vertex_shader.glsl", "Shaders/fragment_shader.glsl");
    Shader sunShader("Shaders/sun_vertex_shader.glsl", "Shaders/sun_fragment_shader.glsl");
    Shader waterShader("Shaders/water_vertex_shader.glsl", "Shaders/water_fragment_shader.glsl");

    // Load Textures
    GLuint tex = loadBMP("Resources/Textures/wood.bmp");
    GLuint tex2 = loadBMP("Resources/Textures/watah.bmp");
    GLuint tex3 = loadBMP("Resources/Textures/orange.bmp");
    GLuint tex4 = loadBMP("Resources/Textures/cat.bmp");
    GLuint tex5 = loadBMP("Resources/Textures/grass.bmp");
    GLuint skyTexID = loadBMP("Resources/Textures/sky.bmp");

    glBindTexture(GL_TEXTURE_2D, skyTexID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glBindTexture(GL_TEXTURE_2D, 0);

    // Texture Vectors
    std::vector<Texture> textures2;
    textures2.push_back(Texture());
    textures2[0].id = tex2;
    textures2[0].type = "texture_diffuse";

    std::vector<Texture> textures3;
    textures3.push_back(Texture());
    textures3[0].id = tex3;
    textures3[0].type = "texture_diffuse";

    std::vector<Texture> textures4;
    textures4.push_back(Texture());
    textures4[0].id = tex4;
    textures4[0].type = "texture_diffuse";

    std::vector<Texture> textures5;
    textures5.push_back(Texture());
    textures5[0].id = tex5;
    textures5[0].type = "texture_diffuse";

    std::vector<Texture> skyTextures;
    skyTextures.push_back(Texture());
    skyTextures[0].id = skyTexID;
    skyTextures[0].type = "texture_diffuse";

    //Load Models
    MeshLoaderObj loader;
    Mesh sun = loader.loadObj("Resources/Models/sphere.obj",textures3);
    Mesh plane = loader.loadObj("Resources/Models/plane.obj", textures5);
    Mesh cat = loader.loadObj("Resources/Models/cat.obj", textures4);
    Mesh skySphere = loader.loadObj("Resources/Models/sphere.obj", skyTextures);
    Mesh waterMesh = generateWaterGrid(50, 1.0f,textures2);

    float titleUpdateTimer = 0.0f;

    while (!window.isPressed(GLFW_KEY_ESCAPE) &&
        glfwWindowShouldClose(window.getWindow()) == 0)
    {
        window.clear();
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processKeyboardInput();

        // Tracker Logic
        titleUpdateTimer += deltaTime;
        if (titleUpdateTimer > 0.2f) {
            glm::vec3 pos = camera.getCameraPosition();
            std::stringstream ss;
            ss << std::fixed << std::setprecision(2);
            ss << "Cat Game 3D | Coordinates: X: " << pos.x << " Y: " << pos.y << " Z: " << pos.z;
            glfwSetWindowTitle(window.getWindow(), ss.str().c_str());
            titleUpdateTimer = 0.0f;
        }

        // Global Matrices
        glm::mat4 ProjectionMatrix = glm::perspective(90.0f, window.getWidth() * 1.0f / window.getHeight(), 0.1f, 10000.0f);
        glm::mat4 ViewMatrix = glm::lookAt(camera.getCameraPosition(), camera.getCameraPosition() + camera.getCameraViewDirection(), camera.getCameraUp());


        //Draw sky 
        sunShader.use();
        glDisable(GL_CULL_FACE);
        glm::mat4 SkyModel = glm::translate(glm::mat4(1.0), camera.getCameraPosition());
        SkyModel = glm::scale(SkyModel, glm::vec3(500.0f, 500.0f, 500.0f));
        glm::mat4 SkyMVP = ProjectionMatrix * ViewMatrix * SkyModel;
        glUniformMatrix4fv(glGetUniformLocation(sunShader.getId(), "MVP"), 1, GL_FALSE, &SkyMVP[0][0]);
        skySphere.draw(sunShader);
        glEnable(GL_CULL_FACE);

        //Draw water
        waterShader.use();

        // Position the water
        glm::vec3 waterPosition = glm::vec3(70.0f, -20.0f, 43.0f);

        glm::mat4 WaterModel = glm::translate(glm::mat4(1.0), waterPosition);
        glm::mat4 WaterMVP = ProjectionMatrix * ViewMatrix * WaterModel;

        glUniformMatrix4fv(glGetUniformLocation(waterShader.getId(), "MVP"), 1, GL_FALSE, &WaterMVP[0][0]);
        glUniform1f(glGetUniformLocation(waterShader.getId(), "time"), (float)glfwGetTime());
        glUniform3f(glGetUniformLocation(waterShader.getId(), "lightColor"), lightColor.x, lightColor.y, lightColor.z);
        glUniform3f(glGetUniformLocation(waterShader.getId(), "lightPos"), lightPos.x, lightPos.y, lightPos.z);
        glUniform3f(glGetUniformLocation(waterShader.getId(), "viewPos"), camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);

        waterMesh.draw(waterShader);

    
        shader.use();

        glUniform3f(glGetUniformLocation(shader.getId(), "lightColor"), lightColor.x, lightColor.y, lightColor.z);
        glUniform3f(glGetUniformLocation(shader.getId(), "lightPos"), lightPos.x, lightPos.y, lightPos.z);
        glUniform3f(glGetUniformLocation(shader.getId(), "viewPos"), camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);
        GLuint MatrixID2 = glGetUniformLocation(shader.getId(), "MVP");
        GLuint ModelMatrixID = glGetUniformLocation(shader.getId(), "model");

        // Plane
        glm::mat4 ModelMatrix = glm::translate(glm::mat4(1.0), glm::vec3(0.0f, -20.0f, 0.0f));
        glm::mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
        glUniformMatrix4fv(MatrixID2, 1, GL_FALSE, &MVP[0][0]);
        glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
        plane.draw(shader);

        //Cat
        ModelMatrix = glm::mat4(1.0);
        MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
        glUniformMatrix4fv(MatrixID2, 1, GL_FALSE, &MVP[0][0]);
        glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
        cat.draw(shader);

        //Sun
        sunShader.use();
        ModelMatrix = glm::translate(glm::mat4(1.0), lightPos);
        MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
        glUniformMatrix4fv(glGetUniformLocation(sunShader.getId(), "MVP"), 1, GL_FALSE, &MVP[0][0]);
        sun.draw(sunShader);  

        window.update();
    }
}

void processKeyboardInput()
{
    float cameraSpeed = 30 * deltaTime;
    if (window.isPressed(GLFW_KEY_W)) camera.keyboardMoveFront(cameraSpeed);
    if (window.isPressed(GLFW_KEY_S)) camera.keyboardMoveBack(cameraSpeed);
    if (window.isPressed(GLFW_KEY_A)) camera.keyboardMoveLeft(cameraSpeed);
    if (window.isPressed(GLFW_KEY_D)) camera.keyboardMoveRight(cameraSpeed);
    if (window.isPressed(GLFW_KEY_R)) camera.keyboardMoveUp(cameraSpeed);
    if (window.isPressed(GLFW_KEY_F)) camera.keyboardMoveDown(cameraSpeed);
    if (window.isPressed(GLFW_KEY_LEFT)) camera.rotateOy(cameraSpeed);
    if (window.isPressed(GLFW_KEY_RIGHT)) camera.rotateOy(-cameraSpeed);
    if (window.isPressed(GLFW_KEY_UP)) camera.rotateOx(cameraSpeed);
    if (window.isPressed(GLFW_KEY_DOWN)) camera.rotateOx(-cameraSpeed);
}

//Helper function to create the high-poly water
Mesh generateWaterGrid(int size, float spacing, std::vector<Texture> tex )
{
    std::vector<Vertex> vertices;
    std::vector<int> indices;
    std::vector<Texture> textures2=tex;

    float offset = (size * spacing) / 2.0f; 

    //Generate vertices
    for (int z = 0; z <= size; ++z) {
        for (int x = 0; x <= size; ++x) {
            Vertex v;
            v.pos.x = (x * spacing) - offset;
            v.pos.y = 0.0f; 
            v.pos.z = (z * spacing) - offset;

            v.normals = glm::vec3(0.0f, 1.0f, 0.0f);
            v.textureCoords = glm::vec2((float)x / size, (float)z / size);

            vertices.push_back(v);
        }
    }

    //Generate indices
    for (int z = 0; z < size; ++z) {
        for (int x = 0; x < size; ++x) {
            int topLeft = (z * (size + 1)) + x;
            int topRight = topLeft + 1;
            int bottomLeft = ((z + 1) * (size + 1)) + x;
            int bottomRight = bottomLeft + 1;

            //Triangle 1
            indices.push_back(topLeft);
            indices.push_back(bottomLeft);
            indices.push_back(topRight);

            //Triangle 2
            indices.push_back(topRight);
            indices.push_back(bottomLeft);
            indices.push_back(bottomRight);
        }
    }

    return Mesh(vertices, indices, textures2);
}