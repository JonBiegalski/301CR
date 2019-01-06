#pragma once
#include <cstdlib>
#include <math.h>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <iostream>
#include <vector>
#include "ResourceManager.h"
#include "engine.h"

namespace GameLogic
{

	void GameStart(GameState* game_state);

	void ClearGame(GameState* game_state);

	void ResetGame(GameState* game_state);
}