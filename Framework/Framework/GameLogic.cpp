#pragma once
#include <cstdlib>
#include <math.h>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <iostream>
#include <vector>
#include "GameLogic.h"

namespace GameLogic
{


	void GameStart(GameState* game_state)
	{
		std::cout << "GAME START";

		/*Entity wall("wall1", "wall", sf::Vector2f(500, 700), game_state->texture_list[2], sf::Vector2f(1.5f, 3.2f));
		game_state->SpawnEntity(wall);*/
		Player player1(0, "Player1", "player", sf::Vector2f(200.0f, 200.0f), game_state->texture_list[0], sf::Color(0, 255, 0));
		game_state->SpawnPlayer(player1);
		Player player2(1, "Player2", "player", sf::Vector2f(800.0f, 200.0f), game_state->texture_list[0], sf::Color(255, 0, 0));
		game_state->SpawnPlayer(player2);


		game_state->game_running = true;
		GameLoop(game_state);
	}

	void ClearGame(GameState* game_state)
	{
		game_state->entity_list.clear();
		game_state->player_list.clear();
		game_state->bullet_list.clear();
	}


	//void ResetGame(GameState* game_state)
	//{
	//	ClearGame(game_state);
	//	//ResourceManager::LoadInLevelLua(game_state, game_state->level_name);

	//	GameStart(game_state);
	//}
}