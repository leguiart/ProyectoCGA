#version 330 core
in vec2 ex_texCoord;
out vec4 color;

uniform sampler2D textura1;

void main()
{
	color = texture(textura1, ex_texCoord)*vec4(1.0f);
    //color = vec4(1.0f); // Set alle 4 vector values to 1.0f
}