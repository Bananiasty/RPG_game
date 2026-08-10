#pragma once
#include <iostream>
#include <vector>
#include "raylib.h"
#include "textureManager.h"


class item;
class enemy;
class player; 

struct inventory
{
private:
	int max_slots = 100;

public:
	std::vector<item*>items;
	void add_item(item* new_item);
	void del_item(item* rm_item);
};

struct chest
{
private:

public:
	Vector3 position;
	enemy* enemy_ptr;
	int slots;
	std::vector<item*>chest_loot;
	std::vector<item*>enemy_loot;
	Model chest_model;

	//KONSTRUKTOR DLA SKRZYNI
	chest(std::vector<item*> ch_l, int slots_number, Vector3 pos)
		: position(pos), enemy_ptr(nullptr), slots(slots_number), chest_loot(ch_l), enemy_loot() 
	{
		chest_model = objects.m_chest;
	}

	// KONSTRUKTOR DLA WROGA
	chest(std::vector<item*> e_l, int slots_number, enemy* e) :
		position(Vector3{ 0.0f, 0.0f, 0.0f }),
		enemy_ptr(e),
		slots(slots_number),
		chest_loot(),
		enemy_loot(e_l){}
	
	~chest();
	
};

class Event;
struct Node
{
	int left_id;
	int right_id;
	int previous_id;

	bool discovered;

	int positionX;
	int positionY;

	int room_x;
	int room_y;

	int room_width;
	int room_length;

	enemy* enemy;
	chest* spawn_chest;
};

struct NodeConfig {
	int id;
	int enemy_id = -1;
	int left = -1;
	int right = -1;
	int previous = -1;
	bool discovered = false;
	chest* s_chest = nullptr;
	Vector2 dungeon_pos = { 0, 0 };
	Vector2 room_size = { 5, 5 };
};

struct collisions
{
	bool blokuj_ruch_plus_x = false;
	bool blokuj_ruch_minus_x = false;
	bool blokuj_ruch_plus_z = false;
	bool blokuj_ruch_minus_z = false;
};


struct enemy_config {
	int id = -1;
	std::string name = "";
	int hp = 20;
	int armor = 0;
	int damage = 0;
	int block_chance = 0;
	int crit_chance = 0;
	int dodge_chance = 0;
	int reduced_head_damage = 0;
	Texture2D texture = {};
	Vector3 position = { 0.0f, 1.0f, 0.0f };
	float rotation = 0.0f;
	int level = 1;
	std::string description = "";
};

//A* pathfinding
struct GridPosition 
{
	int x;
	int z;
};

struct tile_path
{
	int x, z;
	int gCost;
	int hCost;
	int fCost() const { return gCost + hCost; }
	GridPosition parent;
};

struct SpriteRenderParams
{
	Vector3 drawPos;
	float targetWidth;
	float targetHeight;
	float frameWidth;
	float frameHeight;

};
SpriteRenderParams GetGhoulRenderParams(Vector3 entityPos, Texture2D texture);















