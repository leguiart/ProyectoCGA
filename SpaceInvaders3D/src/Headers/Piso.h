#pragma once
#ifndef HEADERS_PISO_H_
#define HEADERS_PISO_H_

#include <glm/glm.hpp>
#include <GL/glew.h>
#include <vector>

#define _USE_MATH_DEFINES
#include <math.h>
#include "definition.h"

class Piso {
public:
	Piso()
	{
	}
	Piso(int slices, int stacks);
	void init();
	void load();
	void render();
	virtual ~Piso();
private:
	std::vector<VertexLightTexture> vertexLT;
	std::vector<GLuint> index;
	int slices;
	int stacks;

	GLuint VAO, VBO, EBO;
};

#endif // !HEADERS_PISO_H_
