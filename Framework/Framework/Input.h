#pragma once
#include <cstdlib>
#include <math.h>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <iostream>
#include <vector>

#define PI 3.14159265
#define SPEED 2.1f

namespace Input
{
	//Returns the magnitude of a vector
	float Magnitude(sf::Vector2f vector);

	//Returns the normalized vector
	sf::Vector2f Normalized(sf::Vector2f vector);
}

class PlayerInput {
public:
	int player_id;
	float horizontal;
	float vertical;

	float GetVertical(int id);
	float GetHorizontal(int id);
	sf::Vector2f GetDirectionNormalized(int id);
	float GetRotation(sf::Vector2f dir);
	bool IsShooting(int id);
	PlayerInput();
	PlayerInput(int id);
};



