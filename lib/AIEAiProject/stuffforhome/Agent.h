#pragma once

#include "Application.h"
#include <glm/glm.hpp>

class Agent
{
public:
	glm::vec3 position;
	bool reachedDestination;

	void Update()
	{

	}

	void CheckIfReachedPlus(glm::vec3 Pos1, glm::vec3 Pos2)
	{
		if (glm::all(glm::lessThanEqual(Pos1, Pos2)))
			reachedDestination = false;
		else
			reachedDestination = true;
	}
	void CheckIfReachedMinus(glm::vec3 Pos1, glm::vec3 Pos2)
	{
		if (glm::all(glm::greaterThanEqual(Pos1, Pos2)))
			reachedDestination = false;
		else
			reachedDestination = true;
	}
};