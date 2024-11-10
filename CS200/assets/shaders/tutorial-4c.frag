#version 410 core

in vec2 vForFun;

uniform vec3 uFillColor;

layout(location = 0) out vec4 fFragClr;

void main()
{
    vec2 green_blue = mix(uFillColor.gb, vForFun, 0.5);
    fFragClr        = vec4(uFillColor.r, green_blue, 1.0);
}
