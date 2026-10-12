#include "inventory/Inventory.h"
#include "inventory/Item.h"
#include <vector>

void inventory::add_item(item* new_item)
{
	if (items.size() < max_slots)
	{
		items.push_back(new_item);
	}
}

void inventory::del_item(item* remove_item)
{
	std::erase(items, remove_item);
}
