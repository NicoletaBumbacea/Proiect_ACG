#pragma once
#include <vector>
#include "Model Loading/mesh.h"
#include "Shaders/shader.h"

class Renderer {
public:
    Mesh generateWaterGrid(int size, float spacing, std::vector<Texture> tex);
    Mesh generateCircularRiver(float radius, float width, int segments, std::vector<Texture> tex);
    void applyLighting(Shader& shader, glm::vec3 lightPos, glm::vec3 lightColor, glm::vec3 viewPos);
};