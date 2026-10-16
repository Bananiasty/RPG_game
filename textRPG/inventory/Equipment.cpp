#include "inventory/Equipment.h"
#include "entities/Player.h"
#include "gamestates.h"
#include "struct.h"

item_type armor::get_type()
{
	return item_type::ARMOR;
}

bool armor::is_equipped()
{
	return this->item_equipped;
}

bool helm::is_equipped() { return armor::is_equipped(); }
bool vest::is_equipped() { return armor::is_equipped(); }
bool gauntlets::is_equipped() { return armor::is_equipped(); }
bool boots::is_equipped() { return armor::is_equipped(); }
bool weapon::is_equipped() { return armor::is_equipped(); }
bool shield::is_equipped() { return armor::is_equipped(); }

void armor::use(player* c)
{
	if (c == nullptr) return;

	if (!this->item_equipped)
	{
		c->bag->del_item(this);
		c->equipped_items->add_item(this);
		this->item_equipped = true;
		this->add_player_defense(c, this->defense_stat);
	}
	else
	{
		c->equipped_items->del_item(this);
		c->bag->add_item(this);
		this->item_equipped = false;
		this->reduce_player_defense(c, this->defense_stat);
	}
}

void helm::use(player* c)
{
	if (this->item_equipped == false && c->equipped_helm != nullptr)
	{
		return;
	}
	armor::use(c);

	if (this->item_equipped)
	{
		c->equipped_helm = this;
	}
	else
	{
		c->equipped_helm = nullptr;
	}
}

void vest::use(player* c)
{
	if (this->item_equipped == false && c->equipped_vest != nullptr) return;

	bool was_equipped = this->is_equipped();
	armor::use(c);

	if (was_equipped == false && this->item_equipped == true)
	{
		//add_player_health(c, this->add_health);
		c->equipped_vest = this;
	}
	else if (was_equipped == true && this->item_equipped == false)
	{
		//this->reduce_player_health(c, this->add_health);
		c->equipped_vest = nullptr;
	}
}

void gauntlets::use(player* c)
{
	if (this->item_equipped == false && c->equipped_gauntlets != nullptr) return;

	bool was_equipped = this->is_equipped();
	armor::use(c);

	if (was_equipped == false && this->item_equipped == true)
	{
		add_player_crit_chance(c, this->crit_chance_mod);
		c->equipped_gauntlets = this;
	}
	else if (was_equipped == true && this->item_equipped == false)
	{
		this->reduce_player_crit_chance(c, this->crit_chance_mod);
		c->equipped_gauntlets = nullptr;
	}
}

void boots::use(player* c)
{
	if (this->item_equipped == false && c->equipped_boots != nullptr) return;

	bool was_equipped = this->is_equipped();
	armor::use(c);

	if (was_equipped == false && this->item_equipped == true)
	{
		add_player_dodge_chance(c, this->dodge_chance_mod);
		c->equipped_boots = this;
	}
	else if (was_equipped == true && this->item_equipped == false)
	{
		this->reduce_player_dodge_chance(c, this->dodge_chance_mod);
		c->equipped_boots = nullptr;
	}
}

void weapon::use(player* c)
{
	if (c == nullptr) return;

	if (!this->item_equipped)
	{
		if (c->limbs.get_limb(this->equip_slot).severely_wounded)
		{
			gamestate::gameLogs.push_back("Cannot hold weapon: Right arm is severely wounded!");
			return;
		}
		if (c->equipped_weapon != nullptr)
			return;
	}

	bool was_equipped = this->is_equipped();
	armor::use(c);

	if (!was_equipped && this->item_equipped)
	{
		this->add_player_damage(c, this->damage_stat, this->equip_slot);
		c->equipped_weapon = this;
	}
	else if (was_equipped && !this->item_equipped)
	{
		this->reduce_player_damage(c, this->damage_stat, this->equip_slot);
		c->equipped_weapon = nullptr;
	}
}

void shield::use(player* c)
{
	if (c == nullptr) return;

	if (!this->item_equipped)
	{
		if (c->limbs.left_arm.severely_wounded)
		{
			gamestate::gameLogs.push_back("Cannot hold shield: Left arm is severely wounded!");
			return;
		}
		if (c->equipped_shield != nullptr)
			return;
	}

	bool was_equipped = this->is_equipped();
	armor::use(c);

	if (was_equipped == false && this->item_equipped == true)
	{
		add_player_block_chance(c, this->block_chance);
		c->equipped_shield = this;
	}
	else if (was_equipped == true && this->item_equipped == false)
	{
		this->reduce_player_block_chance(c, this->block_chance);
		c->equipped_shield = nullptr;
	}
}

item* helm::clone() const { return new helm(*this); }
item* vest::clone() const { return new vest(*this); }
item* gauntlets::clone() const { return new gauntlets(*this); }
item* weapon::clone() const { return new weapon(*this); }
item* boots::clone() const { return new boots(*this); }
item* shield::clone() const { return new shield(*this); }
