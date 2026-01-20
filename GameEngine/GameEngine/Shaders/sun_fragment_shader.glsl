#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform vec3 sunColor;

void main()
{
    
    vec2 debugCoords = gl_FragCoord.xy / 700.0; 
    
    FragColor = vec4(sunColor, 1.0);
}