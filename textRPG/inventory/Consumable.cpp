#include "inventory/Consumable.h"
#include "character.h"
#include "gamestates.h"
#include "struct.h"

item_type other_item::get_type()
{
	return item_type::ITEM;
}

item_type food::get_type()
{
	return item_type::FOOD;
}

void usable::use(player* c)
{
	auto& items = c->bag->items;
	for (auto it = items.begin(); it != items.end(); ++it)
	{
		if (*it == this)
		{
			delete* it;
			items.erase(it);
			break;
		}
	}
}

void other_item::use(player* c)
{
	auto& items = c->bag->items;
	for (auto it = items.begin(); it != items.end(); ++it)
	{
		if (*it == this)
		{
			delete* it;
			items.erase(it);
			break;
		}
	}
}

void food::use(player* c)
{
	usable::use(c);
}

void health_potion::use(player* c)
{
	restore_player_health(c, this->restore_health);
	usable::use(c);
}

item* health_potion::clone() const { return new health_potion(*this); }
item* food::clone() const { return new food(*this); }
item* other_item::clone() const { return new other_item(*this); }
