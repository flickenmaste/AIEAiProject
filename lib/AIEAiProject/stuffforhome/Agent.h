#pragma once

#include "Application.h"
#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include "Node.h"

class Agent
{
public:
	glm::vec3 position;
	bool reachedDestination;
	Node currentNode;

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

	void CheckIfReached(glm::vec3 Pos1, Node Pos2)
	{
		float range = (0.5f * 0.5f);
		float distance = glm::distance2(Pos1, Pos2.position);
		if (distance <= range)
		{
			reachedDestination = true;
			return;
		}
		reachedDestination = false;
	}

	void MoveToNextNode(Node *nextNode)
	{
		reachedDestination = false;
		position = glm::vec3(0, 0, 0);
		currentNode = *nextNode;
	}
};