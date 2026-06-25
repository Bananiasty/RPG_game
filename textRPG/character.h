#pragma once
#include <iostream>
#include <vector>
#include <map>
#include "struct.h"
#include "raylib.h"


class character
{

friend class item;
protected:
	std::string name;
	int health, mana, base_defense, base_damage, block_chance, crit_chance, dodge_chance;
	int max_health, max_mana;
	Texture2D grafika;

	
public:
	bool helm_slot = false;
	bool vest_slot = false;
	bool gauntlets_slot = false;
	bool boots_slot = false;
	bool weapon_slot = false;

	character(std::string n, int hp, int mana, int bdef, int bdmg, int b_ch, int c_ch, int d_ch, Texture2D g);

	bool is_dead();
	int take_damage(int dmg_amount, const character* player_ptr, bool is_crit);
	std::pair<int, bool> calculate_dmg();

	std::string get_name() { return name; }
	
	int get_health() { return health; }
	int get_max_health() { return max_health; }
	int get_max_mana() { return max_mana; }
	int get_damage() { return base_damage; }
	int get_defense() { return base_defense; }
	int get_block_chance() { return block_chance; }
	int get_crit_chance() { return crit_chance; }
	int get_dodge_chance() { return dodge_chance; }
	int get_mana() { return mana; }

	void set_max_health(int val) { max_health = val; }
	void set_max_mana(int val) { max_mana = val; }
	
};
class player :public character
{
private:
	int gold;
	int xp;
	int level;
	int xp_to_level_up=1;


	
	
public:
	enemy* current_enemy = nullptr;
	inventory* bag;
	inventory* equipment;
	inventory* potions;


	int xp_from_enemy_dif;
	player(std::string n, int hp, int mana, int bdef, int bdmg, int b_ch, int c_ch, int d_ch, int xp, int level, Texture2D grafika);

	bool validate_and_set_name(const std::string& new_name);

	int get_xp() { return xp; }
	int get_xp_to_level_up() { return xp_to_level_up; }
	int get_level() { return level; }

	void set_name(const std::string& new_name);

	void take_all_loot(chest* c);
	void take_item(chest* c, item* item);

	void grant_xp();

	void sort_bag();

	void check_level_up();
};

class enemy :public character
{
private:
	std::string intro_text;
	int difficulty;
public:
	Texture2D get_texture() const { return grafika; }

	std::string get_intro_text() { return intro_text; }

	enemy(std::string n, int hp, int mana, int bdef, int bdmg, int b_ch, int c_ch, int d_ch, Texture2D grafika, int dif, std::string intro);

	
	enemy* clone() const;
	int get_dif() { return difficulty; };
};

