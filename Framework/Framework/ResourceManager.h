#pragma once
#include <cstdlib>
#include <math.h>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <iostream>
#include <fstream>
#include <vector>
#include <queue>
#include "engine.h"

extern "C" {
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
}

#include <LuaBridge\LuaBridge.h>



//using namespace luabridge;

std::vector<std::string> getElements(const std::string& table, lua_State* L);


namespace ResourceManager 
{
	//Check if file with address level_dir exists
	//Returns TRUE if it does, FALSE if it doesn't
	bool doesFileExist(std::string level_dir)
	{
		if (GetFileAttributesA(level_dir.c_str()) == INVALID_FILE_ATTRIBUTES || GetFileAttributesA(level_dir.c_str())== 0xFFFFFFFFFFFFFFFF)
		{
			return false;
		}
		else
		{
			return true;
		}
	}


	//Loads the level into the GameState gs from lua file with address level_dir
	//Returns TRUE if level has been loaded successfully, FALSE if it has not been
	bool LoadInLevelLua(GameState* gs, std::string level_dir)
	{
		if (doesFileExist(level_dir))
		{
			std::vector<std::string> wall_list;
			lua_State* F = luaL_newstate();
			luaL_dofile(F, level_dir.c_str());
			luaL_openlibs(F);
			lua_pcall(F, 0, 0, 0);

			wall_list = getElements("wall_list", F);
			luabridge::LuaRef wallRef = luabridge::getGlobal(F, "wall_list");
			float pos_x, pos_y, size_x, size_y;
			for (int i = 0; i < wall_list.size(); i++)
			{
				luabridge::LuaRef wall = wallRef[wall_list.at(i)];
				pos_x = wall["pos_x"].cast<float>();
				pos_y = wall["pos_y"].cast<float>();
				size_x = wall["size_x"].cast<float>();
				size_y = wall["size_y"].cast<float>();

				Entity w("wall", "wall", sf::Vector2f(pos_x, pos_y), gs->texture_list[2], sf::Vector2f(size_x, size_y));
				gs->SpawnEntity(w);
			}
			return true;
		}
		else
		{
			return false;
		}
	}

}



//Read in textures into the gamestate texture_list according to the texturelist.txt file
bool LoadInTextures(GameState* gs)
{

	std::string x;
	std::ifstream myfile;
	myfile.open("texturelist.txt");
	if (myfile.is_open())
	{
		//std::cout << "FILE OPENED, READING:" << std::endl;
		while (!myfile.eof())
		{
			myfile >> x;
			//std::cout << x << std::endl;
			sf::Texture y;
			if (!y.loadFromFile(x))
			{
				std::cout << "Failed to load " + x << std::endl;
			}
			gs->texture_list.push_back(y);
		}
	}
	else
	{
		std::cout << "FILE DOESN'T EXIST" << std::endl;
	}
	myfile.close();
	return true;
}

//CODE TAKEN FROM THE LUA SCRIPTING EXAMPLE
std::vector<std::string> getElements(const std::string& tab, lua_State* L) {

	std::string source =
		"function getElements(tab) "
		"s = \"\""
		"for k, v in pairs(_G[tab]) do "
		"    s = s..k..\"|\" "
		"    end "
		"return s "
		"end";

	luaL_loadstring(L, source.c_str());
	lua_pcall(L, 0, 0, 0);
	lua_getglobal(L, "getElements");
	lua_pushstring(L, tab.c_str());
	lua_pcall(L, 1, 1, 0);


	std::string ans = lua_tostring(L, -1);
	std::vector<std::string> elements;
	std::string temp = "";
	for (unsigned int i = 0; i < ans.size(); i++) {
		if (ans.at(i) != '|') {
			temp += ans.at(i);
		}
		else {
			elements.push_back(temp);
			temp = "";
		}
	}


	int n = lua_gettop(L);
	lua_pop(L, 1);

	return elements;
}