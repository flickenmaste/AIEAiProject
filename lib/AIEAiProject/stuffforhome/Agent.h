#pragma once

#include "Application.h"
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <stdlib.h>
#include <iostream>

#include "Node.h"

#include "Gizmos.h"

class Agent
{
public:
	glm::vec3 position;
	float speed;
	bool reachedDestination;
	Node *currentNode;
	Node *nodeList[4];
	glm::mat4 m_customMatrix;
	bool gotTouched;
	bool hasMined;
	Node *homeMine;
	bool hasTarget;

	void Init()
	{
		m_customMatrix = glm::mat4();
		position = glm::vec3(0, 0, 0);
		speed = 5.0f;
		reachedDestination = false;
		hasTarget = false;
		hasMined = false;
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

	void CheckIfReached(Node *Pos2)
	{
		float range = (0.5f * 0.5f);
		float range2 = (3.0f * 3.0f);
		float distance = glm::distance2(position, Pos2->position);
		if (distance <= range2)
			speed = 2.5f;
		else
			speed = 5.0f;
		if (distance <= range)
		{
			reachedDestination = true;
			hasMined = true;
			Pos2->size -= 0.1f;
			return;
		}
		reachedDestination = false;
	}

	void CheckIfReachedMine()
	{
		float range = (0.5f * 0.5f);
		float range2 = (3.0f * 3.0f);
		float distance = glm::distance2(position, homeMine->position);
		if (distance <= range)
		{
			hasMined = false;
			reachedDestination = true;
			hasTarget = true;
			homeMine->size += 0.1f;
			return;
		}
		reachedDestination = false;
	}

	void MoveToNextNode(Node *nextNode)
	{
		reachedDestination = false;
		//position = glm::vec3(0, 0, 0);
		currentNode = nextNode;
	}

	void MoveToRandomNode()
	{
		if (hasTarget)
		{
			int iter = std::rand() % 4;
			currentNode = nodeList[iter];
			gotTouched = false;
			reachedDestination = false;
			hasTarget = false;
		}
		if (hasMined)
		{
			currentNode = homeMine;
			reachedDestination = false;
		}
	}

	void Move(float a_deltaTime)
	{
		glm::vec3 dir = glm::normalize(currentNode->position - position);
		
		if (!reachedDestination)
			position = position + dir * speed * a_deltaTime;

		if (hasMined)
			position = position + dir * speed * a_deltaTime;
	}

	void CheckAgentMovement()
	{
		if (!reachedDestination && !hasMined)
			CheckIfReached(currentNode);

		if (hasMined)
		{
			CheckIfReachedMine();
		}
	}
};

class fAgent
{
public:
	glm::vec3 position;
	float speed;
	glm::mat4 m_customMatrix;
	glm::mat4 m_customMatrix2;
	glm::mat4 m_customMatrix3;
	Agent *target;
	bool closeToTarget;
	float homeSize;

	glm::vec3 homeMine;
	float homeMineSize;

	bool isHungry;
	float hunger;

	bool hasStolen;

	glm::vec3 home;

	void Init()
	{
		m_customMatrix = glm::mat4();
		position = glm::vec3(20, 0, 0);
		speed = 5.0f;
		isHungry = false;
		hunger = 100.0f;
		home = glm::vec3(20, 0, 10);
		homeSize = 0.3f;
		homeMineSize = 0.3f;
		homeMine = glm::vec3(20, 0, 0);
	}

	void Update(float a_deltaTime)
	{
		CheckHunger();
		CheckIfReached();
		CheckIfReachedHome();
		Starve(a_deltaTime);
		Move(a_deltaTime);
		Gizmos::addSphere(position, 1.0f, 10, 10, glm::vec4(250, 0, 0, 1), &m_customMatrix);
		Gizmos::addSphere(home, homeSize, 10, 10, glm::vec4(0, 250, 0, 1), &m_customMatrix2);
		Gizmos::addSphere(homeMine, homeMineSize, 10, 10, glm::vec4(255, 200, 0, 1), &m_customMatrix3);
	}

	void CheckIfReached()
	{
		float range = (1.0f * 1.0f);
		float range2 = (3.0f * 3.0f);
		float distance = glm::distance2(position, target->position);
		if (distance <= range2)
			speed = 2.5f;
		else
			speed = 5.0f;
		if (distance <= range)
		{
			closeToTarget = true;
			target->gotTouched = true;
			if (target->hasMined)
			{
				target->hasMined = false;
				hasStolen = true;
			}
			return;
		}
		closeToTarget = false;
	}
	
	void CheckIfReachedHome()
	{
		float range = (1.0f * 1.0f);
		float range2 = (3.0f * 3.0f);
		float distance = glm::distance2(position, homeMine);
		if (distance <= range)
		{
			if (hasStolen)
			{
				homeMineSize += 0.1f;
				hasStolen = false;
			}
		}

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
		hunger -= 5.0f * a_deltaTime;
	}

	void Move(float a_deltaTime)
	{
		if (!closeToTarget && isHungry)
		{
			glm::vec3 dir = glm::normalize(target->position - position);
			position = position + dir * speed * a_deltaTime;
		}

		if (!isHungry && !hasStolen)
		{
			speed = 5.0f;
			glm::vec3 dir = glm::normalize(home - position);
			position = position + dir * speed * a_deltaTime;
		}

		if (hasStolen)
		{
			speed = 5.0f;
			glm::vec3 dir = glm::normalize(homeMine - position);
			position = position + dir * speed * a_deltaTime;
		}
	}
};