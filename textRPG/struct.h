#pragma once
#include <iostream>
#include <vector>


class item;
class enemy;
class player; 

struct inventory
{
private:
	int max_slots = 10;

public:
	std::vector<item*>items;
	void add_item(item* new_item);
	void del_item(int index);
};

struct chest
{
private:
	
public:
	int slots;
	std::vector<item*>chest_loot;
	chest(std::vector<item*>ch_l, int slots_number) 
	{ 
		chest_loot = ch_l; 
		slots = slots_number;
	}
	void loot_list();
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

	Event* current_event = nullptr;
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






