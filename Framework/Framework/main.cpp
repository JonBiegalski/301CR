#include <cstdlib>
#include <math.h>
#include <iostream>
#include <vector>
#include <queue>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include "ResourceManager.h"
#include "engine.h"
#include "EventQueue.h"



#define SCREEN_X 1366
#define SCREEN_Y 768

//Resets the GameState game_state
void ResetGame(GameState* game_state)
{
	// A FAILED REMNANT OF TRYING TO WORK IN SENDING THE LEVEL DATA FROM SERVER TO CLIENT
	
	//
	//if (game_state->is_host)
	//{

	//	std::string name;
	//	std::cout << "Which level to load?: " << std::endl;
	//	std::cin >> name;
	//	game_state->level_name = name;
	//	game_state->level_data.level_name = name;
	//	while (!ResourceManager::LoadInLevelLua(game_state, game_state->level_name))
	//	{
	//		std::cout << "LEVEL DOES NOT EXIST!" << std::endl;
	//		std::cout << "Which level to load?: " << std::endl;
	//		std::cin >> name;
	//		game_state->level_name = name;
	//		game_state->level_data.level_name = name;
	//	}
	//	LevelData x = LevelData();
	//	x.level_name = name;
	//	for (int j = 0; j < game_state->entity_list.size(); j++)
	//	{
	//		EntityPacket a = EntityPacket(game_state->entity_list[j].position, game_state->entity_list[j].sprite.getScale());
	//		x.entity_list.push_back(a);
	//	}
	//	for (int i = 0; i < 20; i++)
	//	{

	//		ENetPacket * p = enet_packet_create(&x, sizeof(LevelData), ENET_PACKET_FLAG_RELIABLE);
	//		enet_host_broadcast(game_state->host, 0, p);
	//	}
	//}
	//else
	//{
	//	bool a = true;
	//	while (a)
	//	{
	//		while (enet_host_service(game_state->host, &game_state->event, 1) > 0)
	//		{
	//			switch (game_state->event.type)
	//			{

	//			case ENET_EVENT_TYPE_RECEIVE:
	//				std::cout << "Packet received!\n";
	//				LevelData z = LevelData();
	//				memcpy(&z, game_state->event.packet->data, game_state->event.packet->dataLength);
	//				game_state->level_name = z.level_name;
	//				for (int i = 0; i < z.entity_list.size(); i++)
	//				{
	//					Entity wall = Entity("wall", "wall", z.entity_list[i].pos, game_state->texture_list[2], z.entity_list[i].size);
	//					game_state->SpawnEntity(wall);
	//				}
	//				a = false;
	//				//enet_packet_destroy(game_state->event.packet);
	//				break;

	//				
	//			}

	//		}
	//	}
	//	//std::cout << "not a" << std::endl;
	//	
	//	//ResourceManager::LoadInLevelLua(game_state, game_state->level_name);
	//	
	//}
	////ResourceManager::LoadInLevelLua(game_state, game_state->level_name);

	ClearGame(game_state);

	std::string name;
	std::cout << "Which level to load?: " << std::endl;
	std::cin >> name;
	game_state->level_name = name;
	//game_state->level_data.level_name = name;
	while (!ResourceManager::LoadInLevelLua(game_state, game_state->level_name))
	{
		std::cout << "LEVEL DOES NOT EXIST!" << std::endl;
		std::cout << "Which level to load?: " << std::endl;
		std::cin >> name;
		game_state->level_name = name;
		//game_state->level_data.level_name = name;
	}

	GameStart(game_state);
}


int main()
{
	char ishost;
	bool host;
	std::cout << "ARE YOU HOST? Y/N" << std::endl;
	std::cin >> ishost;
	if (ishost == 'Y')
	{
		host = true;
		std::cout << "Your controls:\n WSAD and J to shoot\n";
	}
	else
	{
		host = false;
		std::cout << "Your controls:\n Arrow Keys and Space to shoot\n";
	}

	GameState* game_state = new GameState(SCREEN_X, SCREEN_Y, "GAME!", host);
	EventQueue event_queue();

	std::cout << "Main start" << std::endl;;
	LoadInTextures(game_state);
	game_state->bg.setPosition(0, 0);
	game_state->bg.setTexture(game_state->texture_list[3]);
	ResetGame(game_state);


	return 0;
}

