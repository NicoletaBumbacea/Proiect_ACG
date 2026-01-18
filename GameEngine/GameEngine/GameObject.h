#pragma once
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include "Model Loading/mesh.h"
#include "Shaders/shader.h"

class GameObject {
public:
    glm::vec3 position;
    glm::vec3 rotation;
    float scale;
    Mesh* mesh;

    GameObject(Mesh* m, glm::vec3 pos = glm::vec3(0.0f), float s = 1.0f);
    virtual ~GameObject();

    glm::mat4 getModelMatrix();
    virtual void draw(Shader& shader, const glm::mat4& view, const glm::mat4& proj);
};