// RakNetTut2_Server.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "RakPeerInterface.h"
#include "BitStream.h"
#include "MessageIdentifiers.h"
#include "stdio.h"
#include <map>

enum MESSAGE_ID
{
	ID_USER_NEW_CLIENT = ID_USER_PACKET_ENUM,
	ID_USER_CLIENT_DISCONNECTED,
	ID_USER_ID,

	ID_USER_CUSTOM_DATA,
	ID_USER_CUSTOM_DATA_TWO,
};

#define SERVER_PORT 12001

int main(int argc, char* argv[])
{
	// simulate lost packets for a bad network -- WHY?
	float packetLoss = 0.0f;

	// start server
	RakNet::RakPeerInterface* raknet = RakNet::RakPeerInterface::GetInstance();

	RakNet::SocketDescriptor desc(SERVER_PORT, nullptr);
	raknet->Startup(16, &desc, 1);
	raknet->SetMaximumIncomingConnections(16);

	// store client IDs
	int newClientID = 0;
	std::map<RakNet::SystemAddress, int> clientIDs;

	printf("Server running...\n");

	bool running = true;
	while (running = true)
	{
		RakNet::Packet* packet = raknet->Receive();
		while (packet != nullptr)
		{
			switch (packet->data[0])
			{
				case ID_NEW_INCOMING_CONNECTION:
				{
											   printf("New client connected %i\n", newClientID);
											   clientIDs[packet->systemAddress] = newClientID++;

											   // tell the client his ID and all others connected
											   RakNet::BitStream outStream;
											   outStream.Write((unsigned char)ID_USER_ID);
											   outStream.Write(newClientID - 1);
											   for (auto& client : clientIDs)
											   if (client.second != (newClientID - 1)) outStream.Write(client.second);
											   raknet->Send(&outStream, HIGH_PRIORITY, RELIABLE_ORDERED, 0, packet->systemAddress, false);

											   // tell other clients
											   RakNet::BitStream outStream2;
											   outStream2.Write((unsigned char)ID_USER_NEW_CLIENT);
											   outStream2.Write(newClientID - 1);
											   raknet->Send(&outStream2, HIGH_PRIORITY, RELIABLE_ORDERED, 0, packet->systemAddress, true);

											   break;
				}
				case ID_DISCONNECTION_NOTIFICATION:
				case ID_CONNECTION_LOST:
				{
										   printf("Client disconnected: %i\n", clientIDs[packet->systemAddress]);

										   // tell clients they disconnected
										   RakNet::BitStream outStream;
										   outStream.Write((unsigned char)ID_USER_CLIENT_DISCONNECTED);
										   outStream.Write(clientIDs[packet->systemAddress]);
										   raknet->Send(&outStream, HIGH_PRIORITY, RELIABLE_ORDERED, 0, packet->systemAddress, true);

										   clientIDs.erase(clientIDs.find(packet->systemAddress));

										   break;
				}
				default:
				{
						   if (packet->data[0] >= ID_USER_CUSTOM_DATA)
						   {
							   // just send the data on!
							   if ((rand() / (float)RAND_MAX) >= packetLoss)
							   {
								   RakNet::BitStream outStream(packet->data, packet->length, false);
								   raknet->Send(&outStream, MEDIUM_PRIORITY, UNRELIABLE, 0, packet->systemAddress, true);
							   }
						   }

						   if (packet->data[0] >= ID_USER_CUSTOM_DATA_TWO)
						   {
							   RakNet::BitStream outStream(packet->data, packet->length, false);
							   raknet->Send(&outStream, HIGH_PRIORITY, RELIABLE_ORDERED, 0, packet->systemAddress, true);
						   }
				}
				break;
			};

			raknet->DeallocatePacket(packet);
			packet = raknet->Receive();
		}
	}

	raknet->Shutdown(1000);

	RakNet::RakPeerInterface::DestroyInstance(raknet);
	
	return 0;
}

