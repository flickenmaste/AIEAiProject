#pragma once

#include "Application.h"
#include <glm/glm.hpp>
#include "Gizmos.h"

class Node
{
public:
	glm::vec3 position;
	glm::mat4	m_customMatrix;
	float size;

	bool mined;

	void Init(glm::vec3 Pos)
	{
		position = Pos;
		size = 0.9f;
	}
	
	void Update()
	{
		Gizmos::addSphere(position, size, 10, 10, glm::vec4(255, 255, 0, 1), &m_customMatrix);
	}
};