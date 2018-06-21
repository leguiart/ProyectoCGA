#pragma once
#ifndef HEADERS_ENEMIES_H
#define HEADERS_ENEMIES_H

#include <float.h>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/random.hpp>
#include "definition.h"

class Enemies {
public:
	Enemies(){
	}
	/*
	Enemies(int number) {
		for(int i = 0; i<number; i++)
			enemies.push_back()
	}
	*/

	std::vector<Enemy> enemies;

};

#endif // !HEADERS_ENEMIES_H
