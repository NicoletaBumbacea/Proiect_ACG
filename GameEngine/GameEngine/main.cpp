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
Mesh generateCircularRiver(float radius, float width, int segments, std::vector<Texture> tex);

float deltaTime = 0.0f;
float lastFrame = 0.0f;

Window window("Game Engine", 800, 800);
Camera camera;

// Fish structure
struct FishData {
    glm::vec3 position;
    glm::vec3 target;   
    float speed;
};

glm::vec3 getRandomRiverPoint() {
 
    glm::vec3 center = glm::vec3(-120.0f, -18.6f, -85.0f);
    float minRadius = 15.0f; 
    float maxRadius = 90.0f; 
    float angle = (rand() % 360) * (3.14159f / 180.0f);
    float radius = minRadius + (rand() % (int)(maxRadius - minRadius));
    float x = center.x + cos(angle) * radius;
    float z = center.z + sin(angle) * radius;
    float y = center.y - 2.0f; 

    return glm::vec3(x, y, z);
}


glm::vec3 lightColor = glm::vec3(1.0f);
glm::vec3 lightPos = glm::vec3(-180.0f, 100.0f, -200.0f);

glm::vec3 catPosition = glm::vec3(0.0f, 0.0f, 0.0f);
float catRotationY = 180.0f;

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
    Shader riverShader("Shaders/river_vertex_shader.glsl", "Shaders/river_fragment_shader.glsl");

    // Load Textures
    GLuint tex = loadBMP("Resources/Textures/wood.bmp");
    GLuint tex2 = loadBMP("Resources/Textures/watah.bmp");
    GLuint tex3 = loadBMP("Resources/Textures/orange.bmp");
    GLuint tex4 = loadBMP("Resources/Textures/cat.bmp");
    GLuint tex5 = loadBMP("Resources/Textures/sand.bmp");
    GLuint skyTexID = loadBMP("Resources/Textures/sky.bmp");
    GLuint boatTexID = loadBMP("Resources/Textures/boat_color.bmp");
    GLuint reedTexID = loadBMP("Resources/Textures/reed.bmp");
    GLuint treeTexID = loadBMP("Resources/Textures/forrest.bmp");
    GLuint fishTexID = loadBMP("Resources/Textures/fih.bmp");

   
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

    std::vector<Texture> boatTextures;
    boatTextures.push_back(Texture());
    boatTextures[0].id = boatTexID;
    boatTextures[0].type = "texture_diffuse";

    std::vector<Texture> reedTextures;
    reedTextures.push_back(Texture());
    reedTextures[0].id = reedTexID;
    reedTextures[0].type = "texture_diffuse";

    std::vector<Texture> treeTextures;
    treeTextures.push_back(Texture());
    treeTextures[0].id = treeTexID;
    treeTextures[0].type = "texture_diffuse";

    std::vector<Texture> fishTextures;
    fishTextures.push_back(Texture());
    fishTextures[0].id = fishTexID;
    fishTextures[0].type = "texture_diffuse";

   
    //Load Models
    MeshLoaderObj loader;
    Mesh sun = loader.loadObj("Resources/Models/sphere.obj",textures3);
    Mesh plane = loader.loadObj("Resources/Models/plane.obj", textures5);
    Mesh cat = loader.loadObj("Resources/Models/cat.obj", textures4);
    Mesh skySphere = loader.loadObj("Resources/Models/sphere.obj", skyTextures);
    Mesh waterMesh = generateWaterGrid(120, 1.0f,textures2);
    Mesh riverMesh = generateCircularRiver(50, 100, 1.0f, textures2);
    Mesh boat = loader.loadObj("Resources/Models/boat.obj", boatTextures);
    Mesh reed = loader.loadObj("Resources/Models/reed.obj", reedTextures);
    Mesh tree = loader.loadObj("Resources/Models/bigtree.obj", treeTextures);
    Mesh fish = loader.loadObj("Resources/Models/fih.obj", fishTextures);


    //Fish data
    std::vector<FishData> schoolOfFish;
    // Create 5 random fish
    for (int i = 0; i < 5; i++) {
        FishData fish;
        fish.position = getRandomRiverPoint(); 
        fish.target = getRandomRiverPoint();   
        fish.speed = 10.0f + (rand() % 10);  
        schoolOfFish.push_back(fish);
    }

    float titleUpdateTimer = 0.0f;

    while (!window.isPressed(GLFW_KEY_ESCAPE) &&
        glfwWindowShouldClose(window.getWindow()) == 0)
    {
        window.clear();
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processKeyboardInput();
        glm::vec3 cameraOffset = glm::vec3(0.0f, 15.0f, 30.0f);
        camera.setCameraPosition(catPosition + cameraOffset);

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
        glm::vec3 waterPosition = glm::vec3(200.0f, -20.0f, 120.0f);
        glm::mat4 WaterModel = glm::translate(glm::mat4(1.0), waterPosition);
        glm::mat4 WaterMVP = ProjectionMatrix * ViewMatrix * WaterModel;
        glUniformMatrix4fv(glGetUniformLocation(waterShader.getId(), "MVP"), 1, GL_FALSE, &WaterMVP[0][0]);
        glUniform1f(glGetUniformLocation(waterShader.getId(), "time"), (float)glfwGetTime());
        glUniform3f(glGetUniformLocation(waterShader.getId(), "lightColor"), lightColor.x, lightColor.y, lightColor.z);
        glUniform3f(glGetUniformLocation(waterShader.getId(), "lightPos"), lightPos.x, lightPos.y, lightPos.z);
        glUniform3f(glGetUniformLocation(waterShader.getId(), "viewPos"), camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);
        waterMesh.draw(waterShader);

        //Draw river
        Mesh riverMesh = generateCircularRiver(50.0f, 100.0f, 100, textures2);
        riverShader.use();
        glm::vec3 riverPos = glm::vec3(-120.0f, -19.5f, -85.0f);
        glm::mat4 RiverModel = glm::translate(glm::mat4(1.0), riverPos);
        glm::mat4 RiverMVP = ProjectionMatrix * ViewMatrix * RiverModel;
        glUniformMatrix4fv(glGetUniformLocation(riverShader.getId(), "MVP"), 1, GL_FALSE, &RiverMVP[0][0]);
        glUniform1f(glGetUniformLocation(riverShader.getId(), "time"), (float)glfwGetTime());
        glUniform3f(glGetUniformLocation(riverShader.getId(), "lightColor"), lightColor.x, lightColor.y, lightColor.z);
        glUniform3f(glGetUniformLocation(riverShader.getId(), "lightPos"), lightPos.x, lightPos.y, lightPos.z);
        glUniform3f(glGetUniformLocation(riverShader.getId(), "viewPos"), camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);
        riverMesh.draw(riverShader);

    
        shader.use();

        glUniform3f(glGetUniformLocation(shader.getId(), "lightColor"), lightColor.x, lightColor.y, lightColor.z);
        glUniform3f(glGetUniformLocation(shader.getId(), "lightPos"), lightPos.x, lightPos.y, lightPos.z);
        glUniform3f(glGetUniformLocation(shader.getId(), "viewPos"), camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);
        GLuint MatrixID2 = glGetUniformLocation(shader.getId(), "MVP");
        GLuint ModelMatrixID = glGetUniformLocation(shader.getId(), "model");


        // Draw plane
        glm::mat4 ModelMatrix = glm::translate(glm::mat4(1.0), glm::vec3(0.0f, -20.0f, 0.0f));
        float planeScale = 3.0f;
        ModelMatrix = glm::scale(ModelMatrix, glm::vec3(planeScale, planeScale, planeScale));
        glm::mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
        glUniformMatrix4fv(MatrixID2, 1, GL_FALSE, &MVP[0][0]);
        glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
        plane.draw(shader);

        //Draw boat
        ModelMatrix = glm::translate(glm::mat4(1.0), glm::vec3(-28.0f, -20.0f, -56.0f));
        float boatScale =0.1f;
        ModelMatrix = glm::scale(ModelMatrix, glm::vec3(boatScale, boatScale, boatScale));
        MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
        glUniformMatrix4fv(MatrixID2, 1, GL_FALSE, &MVP[0][0]);
        glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
        boat.draw(shader);

        // Draw reeds
        std::vector<glm::vec3> reedPositions = {
            glm::vec3(-25.0f, -20.0f, -80.0f),
            glm::vec3(-30.0f, -20.0f, -75.0f),
            glm::vec3(-20.0f, -20.0f, -82.0f),
        };
        for (unsigned int i = 0; i < reedPositions.size(); i++)
        {
            ModelMatrix = glm::mat4(1.0);
            ModelMatrix = glm::translate(ModelMatrix, reedPositions[i]);
            float reedScale = 2.0f;
            ModelMatrix = glm::scale(ModelMatrix, glm::vec3(reedScale, reedScale, reedScale));
            MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
            glUniformMatrix4fv(MatrixID2, 1, GL_FALSE, &MVP[0][0]);
            glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
            reed.draw(shader);
        }

        //Draw trees
        std::vector<glm::vec3> treePositions = {
            glm::vec3(225.0f, -20.0f, -162.0f),
            glm::vec3(205.0f, -20.0f, -175.0f),
            glm::vec3(170.0f, -20.0f, -155.0f),
        };
        for (unsigned int i = 0; i < treePositions.size(); i++)
        {
            ModelMatrix = glm::translate(glm::mat4(1.0), treePositions[i]);
            float treeScale = 3.0f;
            ModelMatrix = glm::scale(ModelMatrix, glm::vec3(treeScale, treeScale, treeScale));
            MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
            glUniformMatrix4fv(MatrixID2, 1, GL_FALSE, &MVP[0][0]);
            glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
            tree.draw(shader);
        }

         // Draw fih 
        for (unsigned int i = 0; i < schoolOfFish.size(); i++)
        {
            glm::vec3 direction = schoolOfFish[i].target - schoolOfFish[i].position;
            float distance = glm::length(direction);

            if (distance < 1.0f) {
            
                schoolOfFish[i].target = getRandomRiverPoint();
            }
            else {
                glm::vec3 moveDir = glm::normalize(direction);
                schoolOfFish[i].position += moveDir * schoolOfFish[i].speed * deltaTime;
            }
            ModelMatrix = glm::mat4(1.0);
            ModelMatrix = glm::translate(ModelMatrix, schoolOfFish[i].position);

            float angle = atan2(direction.x, direction.z);
            ModelMatrix = glm::rotate(ModelMatrix, angle + 1.57f, glm::vec3(0.0f, 1.0f, 0.0f));

            float fishScale = 3.0f;
            ModelMatrix = glm::scale(ModelMatrix, glm::vec3(fishScale, fishScale, fishScale));

            MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
            glUniformMatrix4fv(MatrixID2, 1, GL_FALSE, &MVP[0][0]);
            glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
            fish.draw(shader);
        }

        // Draw cat
        ModelMatrix = glm::mat4(1.0);
        ModelMatrix = glm::translate(ModelMatrix, catPosition);
        ModelMatrix = glm::rotate(ModelMatrix, glm::radians(catRotationY), glm::vec3(0.0f, 1.0f, 0.0f));
        float catScale = 5.0f;
        ModelMatrix = glm::scale(ModelMatrix, glm::vec3(catScale, catScale, catScale));
        MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
        glUniformMatrix4fv(MatrixID2, 1, GL_FALSE, &MVP[0][0]);
        glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
        cat.draw(shader);
       

        // Draw sun
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
    //Define speeds
    float cameraRotationSpeed = 30 * deltaTime;
    float catSpeed = 20.0f * deltaTime;

    // W =Forward (- Z)
    if (window.isPressed(GLFW_KEY_W))
        catPosition.z -= catSpeed;

    // S = Backward (+ Z)
    if (window.isPressed(GLFW_KEY_S))
        catPosition.z += catSpeed;

    // A =Left (- X)
    if (window.isPressed(GLFW_KEY_A))
        catPosition.x -= catSpeed;

    // D = Right (+ X)
    if (window.isPressed(GLFW_KEY_D))
        catPosition.x += catSpeed;

    //Camera Rotation 
    if (window.isPressed(GLFW_KEY_LEFT)) camera.rotateOy(cameraRotationSpeed);
    if (window.isPressed(GLFW_KEY_RIGHT)) camera.rotateOy(-cameraRotationSpeed);
    if (window.isPressed(GLFW_KEY_UP)) camera.rotateOx(cameraRotationSpeed);
    if (window.isPressed(GLFW_KEY_DOWN)) camera.rotateOx(-cameraRotationSpeed);

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


// Helper fct to create a circular river mesh
Mesh generateCircularRiver(float radius, float width, int segments, std::vector<Texture> tex)
{
    std::vector<Vertex> vertices;
    std::vector<int> indices;
    std::vector<Texture> textures2 = tex;

    float innerRadius = radius - (width / 2.0f);

    int widthSegments = 18; 

    for (int i = 0; i <= segments; ++i) {
        float theta = (float)i / segments * 2.0f * 3.1415926f;
        float cosTheta = cos(theta);
        float sinTheta = sin(theta);

        for (int j = 0; j <= widthSegments; ++j) {
            float r = innerRadius + ((float)j / widthSegments) * width;

            Vertex v;
            // polar to cartesian conversion
            v.pos.x = r * cosTheta;
            v.pos.y = 0.0f;
            v.pos.z = r * sinTheta;
            v.normals = glm::vec3(0.0f, 1.0f, 0.0f);
            v.textureCoords = glm::vec2(((float)i / segments) * 20.0f, (float)j / widthSegments);

            vertices.push_back(v);
        }
    }

    // Generate Indices
    for (int i = 0; i < segments; ++i) {
        for (int j = 0; j < widthSegments; ++j) {
            int topLeft = (i * (widthSegments + 1)) + j;
            int topRight = topLeft + 1;
            int bottomLeft = ((i + 1) * (widthSegments + 1)) + j;
            int bottomRight = bottomLeft + 1;

            indices.push_back(topLeft);
            indices.push_back(bottomLeft);
            indices.push_back(topRight);

            indices.push_back(topRight);
            indices.push_back(bottomLeft);
            indices.push_back(bottomRight);
        }
    }

    return Mesh(vertices, indices, textures2);
}
