#pragma once
#include <string>
#include "entities/Character.h"
#include "inventory/Inventory.h"

class enemy;
struct object;
class item;

class player : public character
{
private:
	int gold;
	int xp;
	int level;
	int xp_to_level_up = 1;

public:
	player(std::string n, int hp, int bdef, int b_ch, int c_ch, int d_ch, float m_s, int xp, int level, Texture2D g, Vector3 pos, float rot);
	virtual ~player();

	enemy* current_enemy = nullptr;
	inventory* bag;
	inventory* equipped_items;

	inventory* equipment;
	inventory* food;
	inventory* inv_items;
	inventory* books;

	bool spell_queued = false;

	Texture2D* queued_animation_texture;
	int queued_frame_count;
	float queued_frame_time;

	int xp_from_enemy_dif;

	bool is_dead() override;

	void player_guard();
	void player_attack();

	bool validate_and_set_name(const std::string& new_name);

	int get_xp() { return xp; }
	int get_xp_to_level_up() { return xp_to_level_up; }
	int get_level() { return level; }

	void set_name(const std::string& new_name);

	void take_all_loot(object* c);
	void grant_xp();
	void sort_bag();
	void check_level_up();
	void drop_item(item* item_to_drop);
};
