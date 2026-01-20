#version 330 core
out vec4 FragColor;

in vec3 TexCoords;

uniform samplerCube skybox;
uniform vec3 tintColor;

void main()
{    
    vec4 texColor = texture(skybox, TexCoords);
    FragColor = texColor * vec4(tintColor, 1.0);
}