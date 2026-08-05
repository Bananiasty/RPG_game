#pragma once
#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <utility>
#include "struct.h"
#include "raylib.h"


enum class BodyPart { NONE, TORSO, HEAD, LEFT_ARM, RIGHT_ARM, LEFT_LEG, RIGHT_LEG };

class character
{

friend class item;
protected:
	std::string name;
	int health, mana, base_defense, base_damage, block_chance, crit_chance, dodge_chance;
	int max_health, max_mana;

	Texture2D grafika;
	
	float rotation;

	bool has_head = true;
	bool has_left_arm = true;
	bool has_right_arm = true;
	bool has_left_leg = true;
	bool has_right_leg = true;

	BodyPart hovered_part = BodyPart::NONE;

	static constexpr float FRAME_WIDTH = 64.0f;
	static constexpr float FRAME_HEIGHT = 128.0f;

	Rectangle get_limb_rec(int angle_index, int row_index) const {
		return Rectangle{
			angle_index * FRAME_WIDTH,
			row_index * FRAME_HEIGHT,
			FRAME_WIDTH,
			FRAME_HEIGHT
		};
	}
	
public:
	Vector3 position;
	
	bool helm_slot = false;
	bool vest_slot = false;
	bool gauntlets_slot = false;
	bool boots_slot = false;
	bool weapon_slot = false;

	BodyPart get_hovered_body_part() const { return hovered_part; }
	void set_hovered_body_part(BodyPart part) { hovered_part = part; }

	float queued_damage = 0.0;

	character(std::string n, int hp, int mana, int bdef, int bdmg, int b_ch, int c_ch, int d_ch, Texture2D g, Vector3 pos, float rot);

	bool is_dead();

	int take_damage(int dmg_amount, const character* player_ptr, bool is_crit, bool is_guard);

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

	Vector3 get_position() const { return position; }
	void set_position(Vector3 new_pos) { position = new_pos; }

	bool get_has_head() const { return has_head; }
	bool get_has_left_arm() const { return has_left_arm; }
	bool get_has_right_arm() const { return has_right_arm; }
	bool get_has_left_leg() const { return has_left_leg; }
	bool get_has_right_leg() const { return has_right_leg; }

	void set_has_head(bool state) { has_head = state; }
	void set_has_left_arm(bool state) { has_left_arm = state; }
	void set_has_right_arm(bool state) { has_right_arm = state; }
	void set_has_left_leg(bool state) { has_left_leg = state; }
	void set_has_right_leg(bool state) { has_right_leg = state; }
	
	void draw_composite(Vector2 screen_pos, int angle_index) const {

		DrawTextureRec(grafika, get_limb_rec(angle_index, 0), screen_pos, WHITE);

		Vector2 limb_screen_pos = { screen_pos.x, screen_pos.y - FRAME_HEIGHT };

		if (has_head) {
			DrawTextureRec(grafika, get_limb_rec(angle_index, 1), limb_screen_pos, WHITE);
		}
		if (has_left_arm) {
			DrawTextureRec(grafika, get_limb_rec(angle_index, 1), limb_screen_pos, WHITE);
		}
		if (has_right_arm) {
			DrawTextureRec(grafika, get_limb_rec(angle_index, 1), limb_screen_pos, WHITE);
		}
		if (has_left_leg) {
			DrawTextureRec(grafika, get_limb_rec(angle_index, 1), limb_screen_pos, WHITE);
		}
		if (has_right_leg) {
			DrawTextureRec(grafika, get_limb_rec(angle_index, 1), limb_screen_pos, WHITE);
		}
	}
};

class player :public character
{
private:
	int gold;
	int xp;
	int level;
	int xp_to_level_up=1;

	
public:
	player(std::string n, int hp, int mana, int bdef, int bdmg, int b_ch, int c_ch, int d_ch, int xp, int level, Texture2D grafika, Vector3 pos, float rot);

	enemy* current_enemy = nullptr;
	inventory* bag;
	inventory* equipment;
	inventory* usables;
	inventory* scrolls;

	bool spell_queued = false;
	

	Texture2D* queued_animation_texture;
	int queued_frame_count;
	float queued_frame_time;

	int xp_from_enemy_dif;

	bool is_guard = false;
	bool is_attack = false;

	void player_guard();
	void player_attack();

	bool validate_and_set_name(const std::string& new_name);

	int get_xp() { return xp; }
	int get_xp_to_level_up() { return xp_to_level_up; }
	int get_level() { return level; }

	void set_name(const std::string& new_name);

	void take_all_loot(chest* c);
	void take_item(chest* c, item* it);

	void grant_xp();

	void sort_bag();

	void check_level_up();
};

class enemy :public character
{
private:
	std::string intro_text;
	int difficulty;
	int id_number;
	int room_id_number;

	bool moving = false;
	Vector3 target_tile = { 0.0f, 0.0f, 0.0f };
	Vector3 forward = { 0.0f, 0.0f, 1.0f };

	int last_frame_index = 0;
public:
	int get_last_frame() const { return last_frame_index; }
	void set_last_frame(int frame) { last_frame_index = frame; }

	void set_forward(Vector3 f) { this->forward = f; }
	Vector3 get_forward() const { return this->forward; }
	bool is_moving() const
	{
		return moving;
	}

	void set_is_moving(bool state)
	{
		moving = state;
	}

	void set_target_tile(Vector3 tile)
	{
		target_tile = tile;
	}

	Vector3 get_target_tile() const
	{
		return target_tile;
	}

	std::vector<item*> loot;
	
	
	Texture2D get_texture() const { return grafika; }

	std::string get_intro_text() { return intro_text; }

	enemy(const enemy_config& config);

	
	enemy* clone() const;
	int get_dif() { return difficulty; }
	int get_id() { return id_number; }
	int get_room_id() { return room_id_number; }

	
};

