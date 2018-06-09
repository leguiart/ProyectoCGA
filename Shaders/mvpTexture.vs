#version 330 core
layout(location=0) in vec3 in_position;
layout(location=1) in vec2 in_texCoord;
layout(location=2) in vec2 in2_texCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
//uniform float offset;
//uniform float offset2;

out vec2 ex_texCoord, ex2_texCoord;

void main(){
    gl_Position = projection * view * model * vec4(in_position, 1.0f);
    // Just pass the color through directly.
    ex_texCoord = vec2(-in_texCoord.x, in_texCoord.y);
	ex2_texCoord = vec2(in2_texCoord.x, in2_texCoord.y);
}