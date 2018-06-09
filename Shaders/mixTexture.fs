#version 330 core
in vec3 ex_Color;
in vec2 ex_texCoord;
in vec2 ex2_texCoord;

out vec4 color;

uniform sampler2D textura1;
uniform sampler2D textura2;
uniform sampler2D textura3;
//uniform vec3 colorOffset;

void main()
{
	//color = texture(textura1, ex_texCoord);
	color = mix(texture(textura1, ex_texCoord), texture(textura2, ex2_texCoord), 0.3); //* vec4(colorOffset, 1.0);
}