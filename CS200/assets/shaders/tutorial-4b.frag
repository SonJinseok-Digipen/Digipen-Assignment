#version 410 core

in vec2 vForFun;

uniform vec3 uFillColor;

layout(location = 0) out vec4 fFragClr;

void main()
{
    vec2 red_green = mix(uFillColor.rg, vForFun, 0.5);
    fFragClr       = vec4(red_green, uFillColor.b, 1.0);
}
