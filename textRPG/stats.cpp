#include <iostream>
#include <vector>
#include "character.h"
#include "inventory_class.h"
#include "gamestates.h"
#include "struct.h"


void item::add_player_defense(player* c, int val)
{
	if (c != nullptr)
	{
		c->base_defense += val;
	};
}
void item::add_player_block_chance(player* c, int val)
{
	if (c != nullptr)
	{
		c->block_chance += val;
	};
}
void item::add_player_health(player* c, int val)
{
	if (c != nullptr)
	{
		c->max_health += val;
		c->health += val;
	};
}
void item::add_player_damage(player* c, int val)
{
	if (c != nullptr)
	{
		c->base_damage += val;
	};
}
void item::add_player_crit_chance(player* c, int val)
{
	if (c != nullptr)
	{
		c->crit_chance += val;
	};
}
void item::add_player_dodge_chance(player* c, int val)
{
	if (c != nullptr)
	{
		c->dodge_chance += val;
	};
}

void item::reduce_player_defense(player* c, int val)
{
	if (c != nullptr)
	{
		c->base_defense -= val;
		if (c->base_defense < 0)
			c->base_defense = 0;
	};
}
void item::reduce_player_health(player* c, int val)
{
	if (c != nullptr)
	{
		c->health -= val;
		c->max_health -= val;
		if (c->health < 0)
			c->health = 0;
	};
}


void item::reduce_player_damage(player* c, int val)
{
	if (c != nullptr)
	{
		c->base_damage -= val;
		if (c->base_damage < 0)
			c->base_damage = 0;
	};
}

void item::reduce_player_block_chance(player* c, int val)
{
	if (c != nullptr)
	{
		c->block_chance -= val;
		if (c->block_chance < 0) c->block_chance = 0;
	};
}
void item::reduce_player_crit_chance(player* c, int val)
{
	if (c != nullptr)
	{
		c->crit_chance -= val;
		if (c->crit_chance < 0)
			c->crit_chance = 0;
	};
}

void item::reduce_player_dodge_chance(player* c, int val)
{
	if (c != nullptr)
	{
		c->dodge_chance -= val;
		if (c->dodge_chance < 0)
			c->dodge_chance = 0;
	};
}
void item::restore_player_health(player* c, int val)
{
	if (c != nullptr)
	{
		c->health += val;
		if(c->get_health() > c->get_max_health())
			c->health = c->get_max_health();
	};
	
}

void item::restore_player_mana(player* c, int val)
{
	if (c != nullptr)
	{
		c->mana += val;
		if (c->get_mana() > c->get_max_mana())
			c->mana = c->get_max_mana();
	};
}
void item::reduce_player_mana(player* c, int val)
{
	if (c != nullptr)
	{
		c->mana -= val;
		if (c->get_mana() <= 0)
		{
			c->mana = 0;
		}
	};
}


