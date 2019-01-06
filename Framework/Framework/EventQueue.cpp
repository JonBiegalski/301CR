#include <cstdlib>
#include <math.h>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <iostream>
#include <vector>
#include "EventQueue.h"

Event::Event()
{

}

Event::Event(std::string type_name, Subsystem subsystem_type, std::vector<Player*> event_players)
{
	type = type_name;
	subsystem = subsystem_type;
	players = event_players;
}

void AddEvent(Event ev)
{
	EventQueue& event_q=EventQueue::getInstance();
	event_q.event_queue.push_back(ev);
}

EventQueue::EventQueue()
{
}