#pragma once
#include <cstdlib>
#include <math.h>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <iostream>
#include <vector>
#include <queue>
#include <enet\enet.h>
#include "Input.h"


#define BULLET_SPEED 5.1f

#define SCREEN_X 1366
#define SCREEN_Y 768

#define GUN_TIME 90


class Entity
{
	public:

	sf::Texture texture;
	int texture_id;
	sf::Vector2f position;
	sf::Sprite sprite;
	std::string name;
	std::string tag;
	sf::RectangleShape col;
	Entity();
	Entity(std::string entity_name, std::string entity_tag, sf::Vector2f entity_position, sf::Texture entity_texture);
	Entity(std::string entity_name, std::string entity_tag, sf::Vector2f entity_position, sf::Texture entity_texture, sf::Vector2f size);
	Entity(std::string entity_name, std::string entity_tag, sf::Vector2f entity_position, sf::Texture entity_texture, sf::Color entity_color);
	void SetPosition(sf::Vector2f x);
	virtual void Update();
};

class Bullet : public Entity
{

	public:
	int player_id;
	sf::Vector2f direction;
	sf::Color color;
	float rotation;
	Bullet();
	Bullet(int id, std::string bullet_name, sf::Vector2f bullet_position, sf::Vector2f dir, float rot, sf::Texture bullet_texture, sf::Color bullet_color);
	~Bullet();
	void Update() override;
	void Collide(Entity other);
};

class Player : public Entity
{
public:
	int player_id;
	sf::Vector2f direction;
	sf::Vector2f shooting_direction;
	sf::Color player_color;
	float rotation;
	PlayerInput input;
	bool has_shot;
	int shoot_timer;
	Player();
	Player(int id, std::string player_name, std::string player_tag, sf::Vector2f player_position, sf::Texture player_texture, sf::Color player_color, int input_id);
	void Update() override;
	void Collide(Entity other);
	void Collide(Player other);
	void Collide(Bullet other);
};

struct EntityPacket
{
	sf::Vector2f pos;
	sf::Vector2f size;
	EntityPacket(sf::Vector2f p, sf::Vector2f s)
	{
		pos = p;
		size = s;
	}
};

struct LevelData {
	std::string level_name;
	std::vector<EntityPacket> entity_list;
	LevelData()
	{
		level_name = "";
		entity_list = std::vector<EntityPacket>();
	}
	LevelData(std::string lvl, std::vector<EntityPacket> el)
	{
		level_name = lvl;
		entity_list = el;
	}
	~LevelData()
	{

	}
};

struct PlayerPacket {
	bool is_shooting;
	sf::Vector2f dir;
	sf::Vector2f pos;
	sf::Vector2f shoot_dir;
	PlayerPacket(sf::Vector2f p,sf::Vector2f d, sf::Vector2f shot_d, bool shot)
	{
		pos = p;
		dir = d;
		is_shooting = shot;
		shoot_dir = shot_d;
	}
	PlayerPacket() {};
};

class GameState
{
public:
	sf::RenderWindow game_window;
	std::vector<sf::Texture> texture_list;
	std::vector<sf::Sprite> sprite_list;
	std::vector<Entity> entity_list;
	//LevelData level_data;
	std::vector<Player> player_list;
	std::vector<Bullet> bullet_list;
	sf::Sprite bg;
	bool is_host;
	bool game_running;
	std::string level_name;
	int packet_timer;
	int packet_timer_max;
	ENetAddress address;
	ENetHost * host;
	ENetEvent event;
	ENetPeer *peer;
	ENetPacket* packet;
	GameState() {};
	GameState(int window_width, int window_height, std::string name);
	GameState(int window_width, int window_height, std::string name, bool hosting);
	void SpawnEntity(Entity entity);
	void SpawnPlayer(Player player);
	void SpawnBullet(Bullet bullet);
	void RemovePlayer(int index);
	void RemovePlayer(std::string name);
	void RemoveBullet(int index);
	void RemoveEntity(int index);
	void Update();
	static GameState& getInstance()
	{
		static GameState gs;
		return gs;
	}
	void CheckForCollisions();
};


void GameLoop(GameState* game_State);

void GameStart(GameState* game_state);

void ClearGame(GameState* game_state);