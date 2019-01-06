#pragma once
#include <cstdlib>
#include <math.h>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <iostream>
#include <vector>
#include <queue>
#include "engine.h"


class Event
{
public:

	std::string type;
	enum Subsystem { GameState, Audio, Rendering, Physics };
	Subsystem subsystem;
	std::vector<Entity*> entities;
	std::vector<Player*> players;
	Event();
	//Event(std::string type_name, Subsystem subsystem_type, std::vector<Entity*> event_entities);
	Event(std::string type_name, Subsystem subsystem_type, std::vector<Player*> event_players);

};

//Adds Event ev to the singleton EventQueue
void AddEvent(Event ev);


class EventQueue
{
private:
	EventQueue();

public:
	static EventQueue& getInstance()
	{
		static EventQueue queue;
		return queue;
	}
	std::vector<Event> event_queue;

};