#pragma once

#include "Application.h"
#include <glm/glm.hpp>

#include "Agent.h"
#include "Node.h"

// derived application class that wraps up all globals neatly
class AIEAiProject : public Application
{
public:

	AIEAiProject();
	virtual ~AIEAiProject();

protected:

	virtual bool onCreate(int a_argc, char* a_argv[]);
	virtual void onUpdate(float a_deltaTime);
	virtual void onDraw();
	virtual void onDestroy();

	void MoveAgent();

	glm::mat4	m_cameraMatrix;
	glm::mat4	m_projectionMatrix;
	glm::mat4	m_customMatrix;

	Agent AIPlayer1;
	fAgent AIPlayer2;

	Node Node1;
	Node Node2;
	Node Node3;
	Node Node4;

	Node playerMine;

};