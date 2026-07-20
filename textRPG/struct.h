#pragma once
#include <iostream>
#include <vector>
#include "raylib.h"


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
	void del_item(int index);
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

	//KONSTRUKTOR DLA SKRZYNI
	chest(std::vector<item*> ch_l, int slots_number, Vector3 pos)
		: position(pos), enemy_ptr(nullptr), slots(slots_number), chest_loot(ch_l), enemy_loot() {}

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

struct collisions
{
	bool blokuj_ruch_plus_x = false;
	bool blokuj_ruch_minus_x = false;
	bool blokuj_ruch_plus_z = false;
	bool blokuj_ruch_minus_z = false;
};







