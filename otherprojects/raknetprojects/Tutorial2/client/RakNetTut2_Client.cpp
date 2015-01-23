#include "RakNetTut2_Client.h"
#include "Gizmos.h"
#include "Utilities.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/ext.hpp>

#include "BitStream.h"
#include "MessageIdentifiers.h"
#include "stdio.h"

#define DEFAULT_SCREENWIDTH 1280
#define DEFAULT_SCREENHEIGHT 720

enum MESSAGE_ID
{
	ID_USER_NEW_CLIENT = ID_USER_PACKET_ENUM,
	ID_USER_CLIENT_DISCONNECTED,
	ID_USER_ID,

	ID_USER_CUSTOM_DATA,
	ID_USER_CUSTOM_DATA_TWO,

	ID_USER_POSITION = ID_USER_CUSTOM_DATA,

	ID_USER_BULLET = ID_USER_CUSTOM_DATA_TWO,
};

#define SERVER_PORT 12001

RakNetTut2_Client::RakNetTut2_Client()
{

}

RakNetTut2_Client::~RakNetTut2_Client()
{

}

bool RakNetTut2_Client::onCreate(int a_argc, char* a_argv[]) 
{
	// initialise the Gizmos helper class
	Gizmos::create();

	// create a world-space matrix for a camera
	m_cameraMatrix = glm::inverse( glm::lookAt(glm::vec3(10,10,10),glm::vec3(0,0,0), glm::vec3(0,1,0)) );

	// get window dimensions to calculate aspect ratio
	int width = 0, height = 0;
	glfwGetWindowSize(m_window, &width, &height);

	// create a perspective projection matrix with a 90 degree field-of-view and widescreen aspect ratio
	m_projectionMatrix = glm::perspective(glm::pi<float>() * 0.25f, width / (float)height, 0.1f, 1000.0f);

	// set the clear colour and enable depth testing and backface culling
	glClearColor(0.25f,0.25f,0.25f,1);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	// Raknet shit
	m_raknet = RakNet::RakPeerInterface::GetInstance();

	RakNet::SocketDescriptor desc;
	m_raknet->Startup(1, &desc, 1);
	m_raknet->Connect("127.0.0.1", SERVER_PORT, nullptr, 0);

	m_myID = -1;

	return true;
}

void RakNetTut2_Client::onUpdate(float a_deltaTime) 
{
	// update our camera matrix using the keyboard/mouse
	Utility::freeMovement( m_cameraMatrix, a_deltaTime, 10 );

	// clear all gizmos from last frame
	Gizmos::clear();
	
	// add an identity matrix gizmo
	Gizmos::addTransform( glm::mat4(1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1) );

	// add a 20x20 grid on the XZ-plane
	for ( int i = 0 ; i < 21 ; ++i )
	{
		Gizmos::addLine( glm::vec3(-10 + i, 0, 10), glm::vec3(-10 + i, 0, -10), 
						 i == 10 ? glm::vec4(1,1,1,1) : glm::vec4(0,0,0,1) );
		
		Gizmos::addLine( glm::vec3(10, 0, -10 + i), glm::vec3(-10, 0, -10 + i), 
						 i == 10 ? glm::vec4(1,1,1,1) : glm::vec4(0,0,0,1) );
	}

	// Raknet shit
	RakNet::Packet* packet = m_raknet->Receive();
	while (packet != nullptr)
	{
		// process
		switch (packet->data[0])
		{
			case ID_CONNECTION_REQUEST_ACCEPTED:
			{
												   printf("Connected\n");
												   m_serverAddress = packet->systemAddress;
												   break;
			}
			case ID_USER_ID:
			{
							   RakNet::BitStream input(packet->data, packet->length, true);
							   input.IgnoreBytes(1);

							   input.Read(m_myID);
							   m_players[m_myID] = glm::vec3(0);
							   m_playerBullets[m_myID] = glm::vec3(0);

							   printf("My ID: %i\n", m_myID);

							   int id = -1;
							   while (input.Read(id))
							   {
								   m_players[id] = glm::vec3(0);
								   m_playerBullets[id] = glm::vec3(0);
							   }
							   break;
			}
			case ID_USER_NEW_CLIENT:
			{
									   RakNet::BitStream input(packet->data, packet->length, true);
									   input.IgnoreBytes(1);
									   int id = -1;
									   input.Read(id);
									   printf("New player connected: %i\n", id);
									   m_players[id] = glm::vec3(0);
									   m_playerBullets[id] = glm::vec3(0);
									   break;
			}
			case ID_USER_CLIENT_DISCONNECTED:
			{
												RakNet::BitStream input(packet->data, packet->length, true);
												input.IgnoreBytes(1);
												int id = -1;
												input.Read(id);
												printf("Player disconnected: %i\n", id);

												m_players.erase(m_players.find(id));
												m_playerBullets.erase(m_playerBullets.find(id));
												break;
			}
			case ID_USER_POSITION:
			{
									 // Handle sending player data
									 RakNet::BitStream input(packet->data, packet->length, true);
									 input.IgnoreBytes(1);
									 int id = -1;
									 input.Read(id);
									 glm::vec3 pos(0);
									 input.Read(pos);

									 m_players[id] = pos;

									 break;
			}
			case ID_USER_BULLET:
			{
								   // Handle sending player data
								   RakNet::BitStream input(packet->data, packet->length, true);
								   input.IgnoreBytes(1);
								   int id = -1;
								   input.Read(id);
								   glm::vec3 pos(0);
								   input.Read(pos);

								   printf("Player died: %i\n", id);

								   m_players[id] = pos;

								   break;
			}
		};

		//get next
		m_raknet->DeallocatePacket(packet);
		packet = m_raknet->Receive();
	}

	// Do player movement
	if (m_myID != -1)
	{
		glm::vec3 movement = myPlayer.MovePlayer(m_window);

		//int choice = myBullet.CheckMovement(m_window, &m_playerBullets[m_myID], &m_players[m_myID]);
		//glm::vec3 currPos = m_players[m_myID];
		//glm::vec3 bulletMovement = myBullet.MoveBullet(choice, currPos);
		
		if (glm::length2(movement) > 0)
		{
			m_players[m_myID] = m_players[m_myID] + glm::normalize(movement) * a_deltaTime * 5.0f;

			// Sending player movement
			RakNet::BitStream output;
			output.Write((unsigned char)ID_USER_POSITION);
			output.Write(m_myID);
			output.Write(m_players[m_myID]);

			m_raknet->Send(&output, HIGH_PRIORITY, RELIABLE_ORDERED, 0, m_serverAddress, false);
		}

		if (myPlayer.CheckSpace(m_window))
		{
			for (auto& explode : m_players)
			{
				if (explode.first != m_myID)
				{
					float range = (6.0f * 6.0f);
					float distance = glm::distance2(m_players[m_myID], m_players[explode.first]);
					if (distance <= range)
					{
						m_players[explode.first] = glm::vec3(0, 0, 0);
					}
					
					//m_players[explode.first] = glm::vec3(0, 0, 0);

					// Sending player movement
					RakNet::BitStream output;
					output.Write((unsigned char)ID_USER_BULLET);
					output.Write(explode.first);
					output.Write(m_players[explode.first]);

					m_raknet->Send(&output, HIGH_PRIORITY, RELIABLE_ORDERED, 0, m_serverAddress, false);
				}
			}
		}

		//if (glm::length2(bulletMovement) > 0)
		//{
		//	m_playerBullets[m_myID] = m_playerBullets[m_myID] + glm::normalize(bulletMovement) * a_deltaTime * 5.0f;

		//	// Sending player movement
		//	RakNet::BitStream output;
		//	output.Write((unsigned char)ID_USER_BULLET);
		//	output.Write(m_myID);
		//	output.Write(m_playerBullets[m_myID]);

		//	m_raknet->Send(&output, HIGH_PRIORITY, RELIABLE_ORDERED, 0, m_serverAddress, false);
		//}
	}

	// Draw all players
	for (auto& player : m_players)
		Gizmos::addAABBFilled(player.second, glm::vec3(0.5f), glm::vec4(1, 1, 0, 1));

	//for (auto& bullet : m_playerBullets)
		//Gizmos::addAABBFilled(bullet.second, glm::vec3(0.1f), glm::vec4(250, 0, 0, 1));

	// quit our application when escape is pressed
	if (glfwGetKey(m_window,GLFW_KEY_ESCAPE) == GLFW_PRESS)
		quit();
}

void RakNetTut2_Client::onDraw() 
{
	// clear the backbuffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	// get the view matrix from the world-space camera matrix
	glm::mat4 viewMatrix = glm::inverse( m_cameraMatrix );
	
	// draw the gizmos from this frame
	Gizmos::draw(m_projectionMatrix, viewMatrix);

	// get window dimensions for 2D orthographic projection
	int width = 0, height = 0;
	glfwGetWindowSize(m_window, &width, &height);
	Gizmos::draw2D(glm::ortho<float>(0, width, 0, height, -1.0f, 1.0f));
}

void RakNetTut2_Client::onDestroy()
{
	// Raknet shit
	m_raknet->Shutdown(100);
	RakNet::RakPeerInterface::DestroyInstance(m_raknet);
	
	// clean up anything we created
	Gizmos::destroy();
}

// main that controls the creation/destruction of an application
int main(int argc, char* argv[])
{
	// explicitly control the creation of our application
	Application* app = new RakNetTut2_Client();
	
	if (app->create("AIE - RakNetTut2_Client",DEFAULT_SCREENWIDTH,DEFAULT_SCREENHEIGHT,argc,argv) == true)
		app->run();
		
	// explicitly control the destruction of our application
	delete app;

	return 0;
}