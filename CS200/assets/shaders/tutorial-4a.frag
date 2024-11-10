#version 410 core

uniform vec3 uFillColor;

layout(location = 0) out vec4 fFragClr;

void main()
{
    fFragClr = vec4(uFillColor, 1.0);
}
