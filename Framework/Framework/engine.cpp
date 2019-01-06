#include <cstdlib>
#include <math.h>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <iostream>
#include <vector>
#include <enet\enet.h>
#include "EventQueue.h"
#include "engine.h"


void ResetGame(GameState* game_state);

void GameLoop(GameState* game_state)
{


	//GAME LOOP
	while (game_state->game_window.isOpen())
	{

		sf::Event event;

		game_state->game_window.pollEvent(event);
		if (event.type == sf::Event::Closed)
			game_state->game_window.close();

		game_state->Update();

		//cout << "frame" << endl;
	}


}

void GameStart(GameState* game_state)
{
	std::cout << "GAME START";
	int id;
	if (game_state->is_host)
	{
		id = 0;
	}
	else
	{

		id = 1;
	}

	Player player1(0, "Player1", "player", sf::Vector2f(300.0f, 384.0f), game_state->texture_list[0], sf::Color(0, 255, 0), id);
	game_state->SpawnPlayer(player1);
	Player player2(1, "Player2", "player", sf::Vector2f(1066.0f, 384.0f), game_state->texture_list[0], sf::Color(255, 0, 0), id);
	game_state->SpawnPlayer(player2);


	game_state->game_running = true;
	GameLoop(game_state);
}

//Clears up the GameState, removing all entities, players and bullets
void ClearGame(GameState* game_state)
{
	game_state->entity_list.clear();
	game_state->player_list.clear();
	game_state->bullet_list.clear();
}

//Checks if position is outside the screen
//Returns TRUE if position is outside, FALSE if position is within the screen
bool OutOfBounds(sf::Vector2f vec)
{
	if (vec.x<-5 || vec.y<-5 || vec.x>SCREEN_X || vec.y>SCREEN_Y)
	{
		return true;
	}
	else
	{
		return false;
	}
}



GameState::GameState(int window_width, int window_height, std::string name)
{
	game_window.create(sf::VideoMode(window_width, window_height), name);
	game_window.setFramerateLimit(60);
	//level_data = LevelData();
	player_list = std::vector<Player>();
	bullet_list = std::vector<Bullet>();
	sprite_list = std::vector<sf::Sprite>();
	texture_list = std::vector<sf::Texture>();
	game_running = true;
}

GameState::GameState(int window_width, int window_height, std::string name, bool hosting)
{
	game_window.create(sf::VideoMode(window_width, window_height), name);
	//entity_list = std::vector<Entity>();
	//level_data = LevelData();
	player_list = std::vector<Player>();
	bullet_list = std::vector<Bullet>();
	sprite_list = std::vector<sf::Sprite>();
	texture_list = std::vector<sf::Texture>();
	is_host = hosting;
	game_running = true;
	if (is_host)
	{
		if (enet_initialize() != 0)
		{
			fprintf(stderr, "An error occurred while initializing ENet.\n");
		}

		/* Bind the server to the default localhost.     */
		/* A specific host address can be specified by   */
		/* enet_address_set_host (& address, "x.x.x.x"); */
		//address.host = ENET_HOST_ANY;
		enet_address_set_host(&address, "localhost");
		//enet_address_set_host(&address, "192.168.0.7");
		/* Bind the server to port 1234. */
		address.port = 1234;
		host = enet_host_create(&address /* the address to bind the server host to */,
			32      /* allow up to 32 clients and/or outgoing connections */,
			2      /* allow up to 2 channels to be used, 0 and 1 */,
			0      /* assume any amount of incoming bandwidth */,
			0      /* assume any amount of outgoing bandwidth */);
		if (host == NULL)
		{
			fprintf(stderr,
				"An error occurred while trying to create an ENet server host.\n");
			exit(EXIT_FAILURE);
		}

		char buf[256];
		std::cout << "YOU ARE A HOST WITH ADDRESS: " << address.host << ":" << address.port << std::endl;

		enet_address_get_host_ip(&address, buf, sizeof buf);
		std::cout << buf << std::endl;
		int x = 0;
		while (x == 0)
		{

			while (enet_host_service(host, &event, 1) > 0)
			{
				switch (event.type)
				{
				case ENET_EVENT_TYPE_CONNECT:

					std::cout << "A client connected from address " << event.peer->address.host << ":" << event.peer->address.port << ".\n";

					/* This event type has an associated peer: the client which has
					connected to the server. We can store some data about this peer
					for as long as it remains in scope using the "data" variable. */
					//event.peer->data = "This is a client";
					x++;

					break;
				case ENET_EVENT_TYPE_DISCONNECT:
					std::cout << "The client from address " << event.peer->address.host << ":" << event.peer->address.port << " disconnected \n";

					event.peer->data = NULL;

					break;
				}
			}
		}
	}
	else
	{
		if (enet_initialize() != 0)
		{
			fprintf(stderr, "An error occurred while initializing ENet.\n");
		}

		host = enet_host_create(NULL /* create a client host */,
			1 /* only allow 1 outgoing connection */,
			2 /* allow up 2 channels to be used, 0 and 1 */,
			0 /* assume any amount of incoming bandwidth */,
			0 /* assume any amount of incoming bandwidth */);
		if (host == NULL)
		{
			fprintf(stderr,
				"An error occurred while trying to create an ENet client host.\n");
			exit(EXIT_FAILURE);
		}
		//enet_address_set_host(&address, "localhost");
		enet_address_set_host(&address, "localhost");
		address.port = 1234;
		//enet_address_set_host(&host->address, "localhost");
		//host->address.port = 1234;
		std::cout << "YOU ARE A CLIENT CONNECTING TO " << address.host << ":" << address.port << std::endl;


		/* Initiate the connection, allocating the two channels 0 and 1. */
		peer = enet_host_connect(host, &address, 2, 0);

		if (peer == NULL)
		{
			fprintf(stderr,
				"No available peers for initiating an ENet connection.\n");
			exit(EXIT_FAILURE);
		}


		/* Wait up to 5 seconds for the connection attempt to succeed. */
		if (enet_host_service(host, &event, 5000) > 0 &&
			event.type == ENET_EVENT_TYPE_CONNECT)
		{
			std::cout << "Connection to " << address.host << ":" << address.port << " succeeded." << std::endl;
			bool a = true;
			while (enet_host_service(host, &event, 1) > 0)
			{
				switch (event.type)
				{
				case ENET_EVENT_TYPE_CONNECT:
					printf("A new client connected from %x:%u.\n",
						event.peer->address.host,
						event.peer->address.port);
					/* Store any relevant client information here. */
					//event.peer->data = "Client information";
					break;

				case ENET_EVENT_TYPE_RECEIVE:
					std::cout << "Packet received!\n";
					//memcpy(&level_data, event.packet->data, event.packet->dataLength);
					//enemy.setPosition(sf::Vector2f(newPosition->x, newPosition->y));
					a = false;
					break;

				case ENET_EVENT_TYPE_DISCONNECT:
					printf("%s disconnected.\n", event.peer->data);
					/* Reset the peer's client information. */
					event.peer->data = NULL;
				}
			}/*
			while (a)
			{
				

			}*/
		}
		else
		{
			/* Either the 5 seconds are up or a disconnect event was */
			/* received. Reset the peer in the event the 5 seconds   */
			/* had run out without any significant event.            */
			enet_peer_reset(peer);
			std::cout << "Connection to " << address.host << ":" << address.port << " failed." << std::endl;

		}
	}
}

Entity::Entity()
{

}

Entity::Entity(std::string entity_name, std::string entity_tag, sf::Vector2f entity_position, sf::Texture entity_texture)
{
	name = entity_name;
	tag = entity_tag;
	position = entity_position;
	texture = entity_texture;
	sprite.setTexture(texture);
	sprite.setPosition(entity_position);
	sprite.setOrigin(texture.getSize().x / 2, texture.getSize().y / 2);


	col.setSize(sf::Vector2f(texture.getSize()));
	col.setPosition(entity_position);
	col.setOrigin(col.getSize().x / 2, col.getSize().y / 2);
	//std::cout << "Created entity " + entity_name + " with tag: " << std::endl;
	//FOR DEBUGGING
	//col.setFillColor(sf::Color(200, 200, 200));
}

Entity::Entity(std::string entity_name, std::string entity_tag, sf::Vector2f entity_position, sf::Texture entity_texture, sf::Vector2f size)
{
	name = entity_name;
	tag = entity_tag;
	position = entity_position;
	texture = entity_texture;
	sprite.setTexture(entity_texture);
	sprite.setPosition(entity_position);
	sprite.setScale(size);
	sprite.setOrigin(texture.getSize().x / 2, texture.getSize().y / 2);


	col.setSize(sf::Vector2f(texture.getSize()));
	col.setPosition(entity_position);
	col.setOrigin(col.getSize().x / 2, col.getSize().y / 2);
	col.setScale(size);
	//std::cout << "Created entity " + entity_name + " with tag: " << std::endl;
	//FOR DEBUGGING
	//col.setFillColor(sf::Color(200, 200, 200));
}

Entity::Entity(std::string entity_name, std::string entity_tag, sf::Vector2f entity_position, sf::Texture entity_texture, sf::Color entity_color)
{
	name = entity_name;
	tag = entity_tag;
	position = entity_position;
	texture = entity_texture;
	sprite.setTexture(texture);
	sprite.setOrigin(texture.getSize().x / 2, texture.getSize().y / 2);
	sprite.setColor(entity_color);
	col.setSize(sf::Vector2f(texture.getSize()));
	col.setPosition(entity_position);
	col.setOrigin(col.getSize().x / 2, col.getSize().y / 2);

	//std::cout << "Created entity " + entity_name + " with tag: "+entity_tag << std::endl;
	//FOR DEBUGGING
	//col.setFillColor(sf::Color(200, 200, 200));
}

Player::Player()
{

}

Player::Player(int id,std::string player_name, std::string player_tag, sf::Vector2f player_position, sf::Texture player_texture, sf::Color player_color, int input_id)
{
	player_id = id;
	name = player_name;
	tag = player_tag;
	position = player_position;
	texture = player_texture;
	direction = sf::Vector2f(0, 0);
	sprite.setTexture(texture);
	sprite.setOrigin(texture.getSize().x / 2, texture.getSize().y / 2);
	sprite.setColor(player_color);
	rotation = 0;
	input = PlayerInput(input_id);
	col.setSize(sf::Vector2f(texture.getSize()));
	col.setPosition(player_position);
	col.setOrigin(col.getSize().x / 2, col.getSize().y / 2);
	has_shot = false;
	shooting_direction = sf::Vector2f(1, 0);
	shoot_timer = GUN_TIME;

	//FOR DEBUGGING
	//col.setFillColor(sf::Color(200, 200, 200));
	//std::cout << "Created Player " + player_name + " with tag: "+player_tag << std::endl;
}

Bullet::Bullet()
{

}

Bullet::Bullet(int id, std::string bullet_name, sf::Vector2f bullet_position, sf::Vector2f dir, float rot, sf::Texture bullet_texture, sf::Color bullet_color)
{
	player_id = id;
	name = bullet_name;
	position = bullet_position;
	direction = dir;
	rotation = rot;
	texture = bullet_texture;
	sprite.setTexture(texture);
	sprite.setOrigin(texture.getSize().x / 2, texture.getSize().y / 2);
	sprite.setColor(bullet_color);
	sprite.setRotation(rotation);
	col.setPosition(bullet_position);
	col.setSize(sf::Vector2f(texture.getSize()));
	col.setOrigin(col.getSize().x / 2, col.getSize().y / 2);
	col.setRotation(rotation);
}

Bullet::~Bullet()
{

}

void Entity::SetPosition(sf::Vector2f x)
{
	position = x;
}

//Adds the entity to the entity_list, spawning it
void GameState::SpawnEntity(Entity entity)
{
	entity_list.push_back(entity);
}

//Adds the player to the player_list, spawning it
void GameState::SpawnPlayer(Player player)
{
	player_list.push_back(player);

}

//Adds the bullet to the bullet_list, spawning it
void GameState::SpawnBullet(Bullet bullet)
{
	bullet_list.push_back(bullet);
}

//Remove the player from the player_list
void GameState::RemovePlayer(int index)
{
	player_list.erase(player_list.begin() + index);
}

//Remove the player from the player list
void GameState::RemovePlayer(std::string name)
{
	for (int i = 0; i < player_list.size(); i++)
	{
		if (player_list[i].name == name)
		{
			player_list.erase(player_list.begin() + i);
		}
	}
}

//Remove the entity from the entity_list
void GameState::RemoveEntity(int index)
{
	entity_list.erase(entity_list.begin() + index);
}

//Remove the bullet from the bullet_list
void GameState::RemoveBullet(int index)
{
	bullet_list.erase(bullet_list.begin() + index);
}

//GAMESTATE UPDATE
void GameState::Update()
{
	EventQueue& event_q = EventQueue::getInstance();
	for (int i=0;i<event_q.event_queue.size();i++)
	{
		if (event_q.event_queue[i].subsystem == Event::GameState)
		{
			Event x = event_q.event_queue[i];
			event_q.event_queue.erase(event_q.event_queue.begin()+i);
			if (x.type == "BulletSpawn")
			{
				Bullet b(x.players[0]->player_id, "bullet", x.players[0]->position, x.players[0]->shooting_direction, x.players[0]->rotation, texture_list[1], sf::Color(255,255,255,255));
				//cout << "Bullet spawned!" << endl;
				SpawnBullet(b);
			}
			if (x.type == "RemovePlayer")
			{
				RemovePlayer(x.players[0]->name);
			}
		}
	}
	game_window.clear();
	game_window.draw(bg);
	if (entity_list.size() > 0 || player_list.size()>0 || bullet_list.size()>0)
	{
		for (unsigned i = 0; i < entity_list.size(); i++)
		{
			entity_list[i].Update();
			//DEBUG
			//game_window.draw(player_list[i].col);
			//END DEBUG
			game_window.draw(entity_list[i].sprite);
		}
		for (unsigned i = 0; i < player_list.size(); i++)
		{
			player_list[i].Update();
			//DEBUG
			//game_window.draw(player_list[i].col);
			//END DEBUG
			game_window.draw(player_list[i].sprite);

		}
		for (unsigned i = 0; i < bullet_list.size(); i++)
		{
			if (OutOfBounds(bullet_list[i].position))
			{
				RemoveBullet(i);
				i--;
				//cout << "Bullet erased";
			}
			else
			{

				bullet_list[i].Update();
				game_window.draw(bullet_list[i].sprite);
			}
		}
	}
	CheckForCollisions();
	if (game_running && player_list.size()>1)
	{

		if (is_host)
		{

			while (enet_host_service(host, &event, 0) > 0)
			{
				switch (event.type)
				{
				case ENET_EVENT_TYPE_CONNECT:

					std::cout << "A client connected from address " << event.peer->address.host << ":" << event.peer->address.port << ".\n";

					/* This event type has an associated peer: the client which has
					connected to the server. We can store some data about this peer
					for as long as it remains in scope using the "data" variable. */

					event.peer->data = "This is a client";

					break;
				case ENET_EVENT_TYPE_DISCONNECT:
					std::cout << "The client from address " << event.peer->address.host << ":" << event.peer->address.port << " disconnected \n";

					event.peer->data = NULL;

					break;
				case ENET_EVENT_TYPE_RECEIVE:
					//std::cout << "Packet received!\n";
					if (event.packet->dataLength == sizeof(PlayerPacket))
					{

						PlayerPacket y;
						//Player x;
						memcpy(&y, event.packet->data, event.packet->dataLength);
						//player_list[1].col = x.col;
						player_list[1].position = y.pos;
						////player_list[1].SetPosition(x.position);
						player_list[1].direction = y.dir;
						player_list[1].shooting_direction = y.shoot_dir;
						player_list[1].has_shot = y.is_shooting;
						if (player_list[1].has_shot)
						{

							std::vector<Player*> v;
							v.push_back(&player_list[1]);
							Event x("BulletSpawn", Event::GameState, v);
							EventQueue& event_q = EventQueue::getInstance();
							event_q.event_queue.push_back(x);
						}
					}
					break;
				}
			}

			PlayerPacket pack(player_list[0].position,player_list[0].direction,player_list[0].shooting_direction, player_list[0].has_shot);
			packet = enet_packet_create(&pack, sizeof(PlayerPacket), ENET_PACKET_FLAG_RELIABLE);
			//packet = enet_packet_create(&player_list[0], sizeof(Player), ENET_PACKET_FLAG_RELIABLE);
			enet_host_broadcast(host, 0, packet);

		}
		else
		{
			while (enet_host_service(host, &event, 0) > 0)
			{
				switch (event.type)
				{
				case ENET_EVENT_TYPE_DISCONNECT:
					std::cout << "The client from address " << event.peer->address.host << ":" << event.peer->address.port << " disconnected \n";

					event.peer->data = NULL;

					break;
				case ENET_EVENT_TYPE_RECEIVE:
					//std::cout << "Packet received!\n";
					if (event.packet->dataLength==sizeof(PlayerPacket))
					{

						PlayerPacket x;
						//Player x;
						memcpy(&x, event.packet->data, event.packet->dataLength);
						//player_list[0].col = x.col;
						player_list[0].position = x.pos;
						////player_list[0].SetPosition(x.position);
						player_list[0].direction = x.dir;
						player_list[0].shooting_direction = x.shoot_dir;
						player_list[0].has_shot = x.is_shooting;
						////player_list[0].direction = x.dir;
						if (player_list[0].has_shot)
						{

							std::vector<Player*> v;
							v.push_back(&player_list[0]);
							Event x("BulletSpawn", Event::GameState, v);
							EventQueue& event_q = EventQueue::getInstance();
							event_q.event_queue.push_back(x);
							player_list[0].has_shot = false;
						}
					}


					break;
				}
			}
			PlayerPacket pack(player_list[1].position,player_list[1].direction, player_list[1].shooting_direction, player_list[1].has_shot);
			packet = enet_packet_create(&pack, sizeof(PlayerPacket), ENET_PACKET_FLAG_RELIABLE);

			//packet = enet_packet_create(&player_list[1], sizeof(Player), ENET_PACKET_FLAG_RELIABLE);
			enet_host_broadcast(host, 0, packet);

		}
	}
	
	game_window.display();
	if (game_running && player_list.size() == 1)
	{
		game_running = false;
		std::cout << "GAME WON BY " + player_list[0].name << std::endl;
		std::cout << "PRESS R TO RESET" << std::endl;
	}
	if (!game_running && sf::Keyboard::isKeyPressed(sf::Keyboard::R))
	{
		game_running = true;
		ResetGame(this);
	}
}

//ENTITY UPDATE
void Entity::Update()
{
	//sprite.setTexture(texture);
	sprite.setPosition(position);
	//std::cout << "(" + std::to_string(position.x) + ", " + std::to_string(position.y) + ")" << endl;

}

//PLAYER UPDATE
void Player::Update()
{

	//Checks for input from the correct player (that is player0 when on sever, player1 when as client)
	if (input.player_id == player_id)
	{
		direction = input.GetDirectionNormalized(input.player_id);

		if (direction != sf::Vector2f(0, 0))
		{
			shooting_direction = direction;
		}

		

	}
	//Change the position of player
	position = position + (direction *SPEED);

	//Change sprite and collider position
	sprite.setPosition(position);
	col.setPosition(position);

	//Rotate to correct position if moving
	if (!(direction == sf::Vector2f(0, 0)))
	{
		rotation = input.GetRotation(direction);
		sprite.setRotation(rotation);
		col.setRotation(rotation);
	}

	//Shooting
	if (input.player_id == player_id)
	{
		if (shoot_timer == 0)
		{

			if (input.IsShooting(input.player_id))
			{
				if (!has_shot)
				{
					//Create an event to create a bullet
					std::vector<Player*> vec;
					vec.push_back(this);
					Event x("BulletSpawn", Event::GameState, vec);
					EventQueue& event_q = EventQueue::getInstance();
					event_q.event_queue.push_back(x);
					has_shot = true;
					shoot_timer = GUN_TIME;
				}
			}
			else
			{
				has_shot = false;
			}
			
		}
		else
		{
			if (shoot_timer > 0)
			{
				shoot_timer--;
			}
			has_shot = false;
		}
	}

}

//BULLET UPDATE
void Bullet::Update()
{
	position = position + (direction * BULLET_SPEED );
	sprite.setPosition(position);
	col.setPosition(position);


}

//Check for collision between entities
void GameState::CheckForCollisions()
{
	//Checking whether the colliders intersect (the colliders are SFMLs rectangles the size of the sprite, since I failed to config Box2D proper)
	for (int i = 0; i < player_list.size(); i++)
	{
		for (int j = 0; j < entity_list.size(); j++)
		{
			
			if (player_list[i].col.getGlobalBounds().intersects(entity_list[j].col.getGlobalBounds()))
			{
				player_list[i].Collide(entity_list[j]);
			}
		}
		for (int j = 0; j < bullet_list.size(); j++)
		{
			if (player_list[i].col.getGlobalBounds().intersects(bullet_list[j].col.getGlobalBounds()))
			{
				player_list[i].Collide(bullet_list[j]);
			}
		}
		for (int j = 0; j < player_list.size(); j++)
		{
			if (j != i)
			{

				if (player_list[i].col.getGlobalBounds().intersects(player_list[j].col.getGlobalBounds()))
				{
					player_list[i].Collide(player_list[j]);
				}
			}
		}
	}
	for (int i = 0; i < bullet_list.size(); i++)
	{
		for (int j = 0; j < entity_list.size(); j++)
		{
			
			if (bullet_list[i].col.getGlobalBounds().intersects(entity_list[j].col.getGlobalBounds()))
			{
				bullet_list[i].Collide(entity_list[j]);
			}
		}
	}
}


//Collisions

void Player::Collide(Entity other)
{
	//cout << "COLLIDED WITH ENTITY " + other.name << endl;
	if (other.tag == "wall")
	{
		//get normalized vector from wall to player
		//Move player away from wall along this vector of player_speed length
		sf::Vector2f vec = Input::Normalized(position - other.position);
		sf::Vector2f v = direction * -1.2f;
		position = position + v * SPEED;
		sprite.setPosition(position);
		col.setPosition(position);
	}
}

void Player::Collide(Player other)
{

	std::cout << "COLLIDED WITH PLAYER " + other.name << std::endl;
	sf::Vector2f vec = Input::Normalized(position - other.position);
	position = position + vec * SPEED;
	sprite.setPosition(position);
	col.setPosition(position);
}

void Player::Collide(Bullet other)
{
	if (other.player_id != player_id)
	{
		//cout << "COLLIDED WITH BULLET " + other.name << endl;
		std::vector<Player*> vec;
		vec.push_back(this);
		Event x("RemovePlayer", Event::GameState, vec);
		EventQueue& event_q = EventQueue::getInstance();
		event_q.event_queue.push_back(x);
	}
	else
	{
	}
}

void Bullet::Collide(Entity other)
{
	//cout << "COLLIDED WITH ENTITY " + other.name << endl;
	if (other.tag == "wall")
	{
		position = sf::Vector2f(-100, -100);
		sprite.setPosition(position);
		col.setPosition(position);
	}/*
	if (other.tag == "destructable")
	{

	}*/
}