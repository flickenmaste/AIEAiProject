#pragma once

#include "Application.h"
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <stdlib.h>

#include "Node.h"

#include "Gizmos.h"

class Agent
{
public:
	glm::vec3 position;
	bool reachedDestination;
	Node currentNode;
	Node *nodeList[4];
	glm::mat4 m_customMatrix;
	bool gotTouched;

	void Init()
	{
		m_customMatrix = glm::mat4();
		position = glm::vec3(0, 0, 0);
		reachedDestination = false;
		srand(time(NULL));
	}

	void Update(float a_deltaTime)
	{
		CheckAgentMovement();
		Move(a_deltaTime);
		MoveToRandomNode();
		Gizmos::addSphere(position, 1.0f, 10, 10, glm::vec4(1, 1, 1, 1), &m_customMatrix);
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

	void CheckIfReached(Node Pos2)
	{
		float range = (0.5f * 0.5f);
		float distance = glm::distance2(position, Pos2.position);
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
		//position = glm::vec3(0, 0, 0);
		currentNode = *nextNode;
	}

	void MoveToRandomNode()
	{
		if (gotTouched)
		{
			int iter = std::rand() % 4;
			currentNode = *nodeList[iter];
			gotTouched = false;
			reachedDestination = false;
		}
	}

	void Move(float a_deltaTime)
	{
		glm::vec3 dir = glm::normalize(currentNode.position - position);
		
		if (!reachedDestination)
			position = position + dir * 5.0f * a_deltaTime;
	}

	void CheckAgentMovement()
	{
		if (!reachedDestination)
			CheckIfReached(currentNode);
	}
};

class fAgent
{
public:
	glm::vec3 position;
	glm::mat4 m_customMatrix;
	glm::mat4 m_customMatrix2;
	glm::mat4 m_customMatrix3;
	Agent *target;
	bool closeToTarget;

	bool isHungry;
	float hunger;

	glm::vec3 home;

	void Init()
	{
		m_customMatrix = glm::mat4();
		position = glm::vec3(20, 0, 0);
		isHungry = false;
		hunger = 100.0f;
		home = glm::vec3(20, 0, 10);
	}

	void Update(float a_deltaTime)
	{
		CheckHunger();
		CheckIfReached();
		Starve(a_deltaTime);
		Move(a_deltaTime);
		Gizmos::addSphere(position, 1.0f, 10, 10, glm::vec4(250, 0, 0, 1), &m_customMatrix);
		Gizmos::addSphere(home, 0.3f, 10, 10, glm::vec4(0, 250, 0, 1), &m_customMatrix2);
	}

	void CheckIfReached()
	{
		float range = (1.0f * 1.0f);
		float distance = glm::distance2(position, target->position);
		if (distance <= range)
		{
			closeToTarget = true;
			target->gotTouched = true;
			return;
		}
		closeToTarget = false;
	}

	void CheckHunger()
	{
		if (hunger <= 50.0f)
		{
			isHungry = true;
		}
		else if (hunger > 50.0f)
		{
			isHungry = false;
		}

		if (closeToTarget)
			hunger = 100.0f;
	}

	void Starve(float a_deltaTime)
	{
		hunger -= 10.0f * a_deltaTime;
	}

	void Move(float a_deltaTime)
	{
		if (!closeToTarget && isHungry)
		{
			glm::vec3 dir = glm::normalize(target->position - position);
			position = position + dir * 5.0f * a_deltaTime;
		}

		if (!isHungry)
		{
			glm::vec3 dir = glm::normalize(home - position);
			position = position + dir * 5.0f * a_deltaTime;
		}
	}
};