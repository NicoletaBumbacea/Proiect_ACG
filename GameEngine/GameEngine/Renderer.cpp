#include "Renderer.h"

void Renderer::applyLighting(Shader& shader, glm::vec3 lightPos, glm::vec3 lightColor, glm::vec3 viewPos) {
    shader.use();
    glUniform3f(glGetUniformLocation(shader.getId(), "lightColor"), lightColor.x, lightColor.y, lightColor.z);
    glUniform3f(glGetUniformLocation(shader.getId(), "lightPos"), lightPos.x, lightPos.y, lightPos.z);
    glUniform3f(glGetUniformLocation(shader.getId(), "viewPos"), viewPos.x, viewPos.y, viewPos.z);
}

Mesh Renderer::generateWaterGrid(int size, float spacing, std::vector<Texture> tex) {
    std::vector<Vertex> vertices;
    std::vector<int> indices;
    float offset = (size * spacing) / 2.0f;
    for (int z = 0; z <= size; ++z) {
        for (int x = 0; x <= size; ++x) {
            Vertex v;
            v.pos = glm::vec3((x * spacing) - offset, 0.0f, (z * spacing) - offset);
            v.normals = glm::vec3(0.0f, 1.0f, 0.0f);
            v.textureCoords = glm::vec2((float)x / size, (float)z / size);
            vertices.push_back(v);
        }
    }
    for (int z = 0; z < size; ++z) {
        for (int x = 0; x < size; ++x) {
            int tl = (z * (size + 1)) + x;
            int tr = tl + 1;
            int bl = ((z + 1) * (size + 1)) + x;
            int br = bl + 1;
            indices.insert(indices.end(), { tl, bl, tr, tr, bl, br });
        }
    }
    return Mesh(vertices, indices, tex);
}

Mesh Renderer::generateCircularRiver(float radius, float width, int segments, std::vector<Texture> tex) {
    std::vector<Vertex> vertices;
    std::vector<int> indices;
    float innerRadius = radius - (width / 2.0f);
    int widthSegments = 18;
    for (int i = 0; i <= segments; ++i) {
        float theta = (float)i / segments * 2.0f * 3.1415926f;
        for (int j = 0; j <= widthSegments; ++j) {
            float r = innerRadius + ((float)j / widthSegments) * width;
            Vertex v;
            v.pos = glm::vec3(r * cos(theta), 0.0f, r * sin(theta));
            v.normals = glm::vec3(0.0f, 1.0f, 0.0f);
            v.textureCoords = glm::vec2(((float)i / segments) * 20.0f, (float)j / widthSegments);
            vertices.push_back(v);
        }
    }
    for (int i = 0; i < segments; ++i) {
        for (int j = 0; j < widthSegments; ++j) {
            int tl = (i * (widthSegments + 1)) + j;
            int tr = tl + 1;
            int bl = ((i + 1) * (widthSegments + 1)) + j;
            int br = bl + 1;
            indices.insert(indices.end(), { tl, bl, tr, tr, bl, br });
        }
    }
    return Mesh(vertices, indices, tex);
}