#pragma once

#include "Application.h"
#include <glm/glm.hpp>

#include "Gizmos.h"
#include "Utilities.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/ext.hpp>

class Bullet
{
public:

	int CheckMovement(GLFWwindow *m_window, glm::vec3 *bullPos, glm::vec3 *playPos)
	{
		int choice = 0;
		if (glfwGetKey(m_window, GLFW_KEY_LEFT) == GLFW_PRESS)
		{
			*bullPos = *playPos;
			choice = 1;
		}
		if (glfwGetKey(m_window, GLFW_KEY_RIGHT) == GLFW_PRESS)
		{
			*bullPos = *playPos;
			choice = 2;
		}
		if (glfwGetKey(m_window, GLFW_KEY_UP) == GLFW_PRESS)
		{
			*bullPos = *playPos;
			choice = 3;
		}
		if (glfwGetKey(m_window, GLFW_KEY_DOWN) == GLFW_PRESS)
		{
			*bullPos = *playPos;
			choice = 4;
		}

		return choice;
	}

	glm::vec3 MoveBullet(int choice, glm::vec3 movement)
	{
		if (choice == 1)
			movement.x -= 1;
		if (choice == 2)
			movement.x += 1;
		if (choice == 3)
			movement.z -= 1;
		if (choice == 4)
			movement.z += 1;

		return movement;
	}

};