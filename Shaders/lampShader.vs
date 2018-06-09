#version 330 core
layout (location = 0) in vec3 position;
layout(location=1) in vec2 in_texCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec2 ex_texCoord; //ex2_texCoord;

void main()
{
    gl_Position = projection * view * model * vec4(position, 1.0f);
	ex_texCoord = vec2(-in_texCoord.x, in_texCoord.y);
	//ex2_texCoord = vec2(in2_texCoord.x + offset, in2_texCoord.y + offset2);
} 