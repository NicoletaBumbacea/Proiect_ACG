#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;
out float WaterHeight; 

uniform mat4 model;
uniform mat4 MVP;
uniform float time;

float calculateWave(vec2 direction, float amplitude, float frequency, float speed, vec3 currentPos) {
    float wavePhase = dot(direction, currentPos.xz) * frequency + time * speed;
    float sineValue = (sin(wavePhase) + 1.0) / 2.0;
    return 2.0 * amplitude * pow(sineValue, 2.0);
}

void main()
{
    vec3 pos = aPos;
    float totalHeight = 0.0;

    //Calculate Waves Sum
    totalHeight += calculateWave(normalize(vec2(1.0, 0.5)), 0.5, 1.0, 2.0, pos);
    totalHeight += calculateWave(normalize(vec2(-0.7, 0.7)), 0.25, 1.2, 1.5, pos);
    totalHeight += calculateWave(normalize(vec2(0.0, 1.0)), 0.1, 2.5, 3.0, pos);

    float distX = min(aTexCoords.x, 1.0 - aTexCoords.x);
    float distZ = min(aTexCoords.y, 1.0 - aTexCoords.y);
    float distFromEdge = min(distX, distZ);
    float edgeFactor = smoothstep(0.0, 0.05, distFromEdge);
    
  
    pos.y = totalHeight * edgeFactor;
  

    WaterHeight = pos.y; 

    FragPos = vec3(model * vec4(pos, 1.0));
    Normal = normalize(vec3(0.0, 1.0, 0.0)); 
    TexCoords = aTexCoords;
    gl_Position = MVP * vec4(pos, 1.0);
}