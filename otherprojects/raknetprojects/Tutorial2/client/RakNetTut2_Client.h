#pragma once

#include "Application.h"
#include <glm/glm.hpp>

#include "RakPeerInterface.h"
#include <map>
#include "Player.h"
#include "Bullet.h"

// derived application class that wraps up all globals neatly
class RakNetTut2_Client : public Application
{
public:

	RakNetTut2_Client();
	virtual ~RakNetTut2_Client();

protected:

	virtual bool onCreate(int a_argc, char* a_argv[]);
	virtual void onUpdate(float a_deltaTime);
	virtual void onDraw();
	virtual void onDestroy();

	glm::mat4	m_cameraMatrix;
	glm::mat4	m_projectionMatrix;

	// Raknet shit
	RakNet::SystemAddress m_serverAddress;
	RakNet::RakPeerInterface* m_raknet;

	// self id and container for all other players on server
	int m_myID;
	std::map<int, glm::vec3> m_players;
	std::map<int, glm::vec3> m_playerBullets;

	Player myPlayer;
	Bullet myBullet;
};