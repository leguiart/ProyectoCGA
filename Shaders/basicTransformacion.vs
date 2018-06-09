#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 color;

out vec3 ourColor;

uniform mat4 transformacion;
uniform float pos;

void main()
{
    gl_Position = transformacion * vec4(position.x + pos, position.y, position.z, 1.0f);
    ourColor = color;
}