#include "GameObject.h"
#include <gtc/matrix_transform.hpp> 

GameObject::GameObject(Mesh* m, glm::vec3 pos, float s)
    : mesh(m), position(pos), scale(s), rotation(glm::vec3(0.0f)) {
}

GameObject::~GameObject() {
}

glm::mat4 GameObject::getModelMatrix() {
    glm::mat4 model = glm::translate(glm::mat4(1.0f), position);
    model = glm::rotate(model, rotation.x, glm::vec3(1, 0, 0));
    model = glm::rotate(model, rotation.y, glm::vec3(0, 1, 0));
    model = glm::rotate(model, rotation.z, glm::vec3(0, 0, 1));
    model = glm::scale(model, glm::vec3(scale));
    return model;
}

void GameObject::draw(Shader& shader, const glm::mat4& view, const glm::mat4& proj) {
    glm::mat4 model = getModelMatrix();
    glm::mat4 mvp = proj * view * model;

    glUniformMatrix4fv(glGetUniformLocation(shader.getId(), "MVP"), 1, GL_FALSE, &mvp[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(shader.getId(), "model"), 1, GL_FALSE, &model[0][0]);

    mesh->draw(shader);
}