#version 330 core
layout(location=0) in vec3 in_position;
layout(location=1) in vec3 in_color;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 ex_Color;

void main(){
    gl_Position = projection * view * model * vec4(in_position, 1.0f);
    // Just pass the color through directly.
	ex_Color = in_color;
}
