#pragma once

#include "Application.h"
#include <glm/glm.hpp>

#include "RakPeerInterface.h"

// derived application class that wraps up all globals neatly
class RaknetTut1_Client : public Application
{
public:

	RaknetTut1_Client();
	virtual ~RaknetTut1_Client();

protected:

	virtual bool onCreate(int a_argc, char* a_argv[]);
	virtual void onUpdate(float a_deltaTime);
	virtual void onDraw();
	virtual void onDestroy();

	glm::mat4	m_cameraMatrix;
	glm::mat4	m_projectionMatrix;

	// Raknet shit
	RakNet::RakPeerInterface* m_pInterface;
	RakNet::SystemAddress m_ServerAddress;
};