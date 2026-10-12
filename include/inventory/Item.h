#pragma once
#include <string>
#include "raylib.h"
#include "combat/BodyPart.h"

class player;

enum class item_type { ARMOR, ITEM, FOOD, BOOK };

class item
{
protected:
	void add_player_defense(player* c, int val);
	//void add_player_health(player* c, int val);
	void add_player_damage(player* c, int val, BodyPart part);
	void add_player_block_chance(player* c, int val);
	void add_player_crit_chance(player* c, int val);
	void add_player_dodge_chance(player* c, int val);
	void add_player_head_damage_reduction(player* c, int val);

	void reduce_player_defense(player* c, int val);
	//void reduce_player_health(player* c, int val);
	void reduce_player_damage(player* c, int val, BodyPart part);
	void reduce_player_block_chance(player* c, int val);
	void reduce_player_crit_chance(player* c, int val);
	void reduce_player_dodge_chance(player* c, int val);
	//void reduce_player_mana(player* c, int val);
	void reduce_player_head_damage_reduction(player* c, int val);

	void restore_player_health(player* c, int val);
	//void restore_player_mana(player* c, int val);

private:
	std::string name;
	std::string rarity;
	int cost = 0;

public:
	Texture2D* icon_texture;
	item(std::string n, std::string r, int c, Texture2D* icon) : name(n), rarity(r), cost(c), icon_texture(icon) {}
	virtual item* clone() const = 0;
	virtual ~item() {}
	virtual item_type get_type() = 0;
	virtual void use(player* c) {}
	std::string get_name();
	std::string get_rarity() const { return rarity; }
	int get_cost() const { return cost; }
	virtual bool is_equipped() { return false; }

	// EQUIPMENT STATS
	virtual int get_defense_stat() const { return 0; }
	virtual int get_attack_stat() const { return 0; }

	// FOOD STATS
	virtual int get_restore_health() const { return 0; }
	//virtual int get_restore_hunger() const { return 0; }
};
