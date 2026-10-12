#pragma once
#include <vector>

class item;

struct inventory
{
private:
	int max_slots = 100;
public:
	std::vector<item*> items;
	void add_item(item* new_item);
	void del_item(item* rm_item);
};
