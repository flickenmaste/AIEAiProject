#pragma once

#include "Application.h"
#include <glm/glm.hpp>

#include "Gizmos.h"
#include "Utilities.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/ext.hpp>

class Player
{
public:

	void Update(GLFWwindow *m_window)
	{
		MovePlayer(m_window);
	}

	glm::vec3 MovePlayer(GLFWwindow *m_window)
	{
		glm::vec3 movement(0);
		if (glfwGetKey(m_window, GLFW_KEY_LEFT) == GLFW_PRESS)
			movement.x -= 1;
		if (glfwGetKey(m_window, GLFW_KEY_RIGHT) == GLFW_PRESS)
			movement.x += 1;
		if (glfwGetKey(m_window, GLFW_KEY_UP) == GLFW_PRESS)
			movement.z -= 1;
		if (glfwGetKey(m_window, GLFW_KEY_DOWN) == GLFW_PRESS)
			movement.z += 1;

		return movement;
	}

	bool CheckSpace(GLFWwindow *m_window)
	{
		if (glfwGetKey(m_window, GLFW_KEY_SPACE) == GLFW_PRESS)
			return true;
		else
			return false;
	}

};