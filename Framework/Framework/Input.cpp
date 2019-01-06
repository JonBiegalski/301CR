#include <cstdlib>
#include <math.h>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <iostream>
#include <vector>
#include "Input.h"

namespace Input
{

	float Magnitude(sf::Vector2f vector)
	{
		return (sqrtf(pow(vector.x, 2) + pow(vector.y, 2)));
	}


	sf::Vector2f Normalized(sf::Vector2f vector)
	{
		float magnitude = Magnitude(vector);
		sf::Vector2f new_vector(vector.x / magnitude, vector.y / magnitude);
		return new_vector;
	}
}



float PlayerInput::GetVertical(int id)
{
	float x = 0;
	if (id == 0)
	{

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
		{
			x--;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
		{
			x++;
		}
	}
	if (id == 1)
	{
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
		{
			x--;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
		{
			x++;
		}
	}
	return x;
}

float PlayerInput::GetHorizontal(int id)
{
	float x = 0;
	if (id == 0)
	{

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
		{
			x--;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
		{
			x++;
		}
	}
	if (id == 1)
	{
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
		{
			x--;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
		{
			x++;
		}
	}
	return x;
}

sf::Vector2f PlayerInput::GetDirectionNormalized(int id)
{
	if (GetHorizontal(id) == 0.0f && GetVertical(id) == 0.0f)
	{
		return sf::Vector2f(0, 0);
	}
	else
	{
		return sf::Vector2f(Input::Normalized(sf::Vector2f(GetHorizontal(id), GetVertical(id))));
	}
}

float PlayerInput::GetRotation(sf::Vector2f dir)
{
	double x, y, value;
	float z;
	x = dir.x;
	y = dir.y;
	value = 180.0 / PI;
	z = atan2(y, x) * value;
	return z;
}

bool PlayerInput::IsShooting(int id)
{
	if (id == 0)
	{
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::J))
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	
}

PlayerInput::PlayerInput()
{


}

PlayerInput::PlayerInput(int id)
{
	player_id = id;
	horizontal = 0;
	vertical = 0;
}