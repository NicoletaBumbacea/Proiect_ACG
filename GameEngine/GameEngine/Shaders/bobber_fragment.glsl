#version 330 core
out vec4 FragColor;

in float HeightPos;

void main()
{
    if(HeightPos > 0.0)
    {
         FragColor = vec4(1.0, 0.0, 0.0, 1.0);
    }
    else
    {
         FragColor = vec4(1.0, 1.0, 1.0, 1.0);
    }
}