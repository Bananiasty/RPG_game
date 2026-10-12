#pragma once
#include <string>
#include <vector>
#include <utility>
#include "raylib.h"
#include "combat/BodyPart.h"
#include "combat/Limb.h"

class gamestate;
class exploration;
class player;
class helm;
#include "inventory/Item.h"
class vest;
class gauntlets;
class boots;
class weapon;
class shield;

class character
{
friend class item;
protected:
	std::string name;
	int base_defense;
	int block_chance, crit_chance, dodge_chance;
	int reduced_head_damage;
	int current_health, max_health;

	Texture2D grafika;
	float rotation;

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
	gamestate* gs;
	exploration* exp;

	limbs_struct limbs;
	Vector3 position;

	bool is_guard = false;
	bool is_attack = false;

	bool is_bleeding = false;
	int applied_bleed_damage = 0;
	int bleed_status_timer = 0;
	bool just_applied_bleed = false;

	helm* equipped_helm = nullptr;
	vest* equipped_vest = nullptr;
	gauntlets* equipped_gauntlets = nullptr;
	boots* equipped_boots = nullptr;
	weapon* equipped_weapon = nullptr;
	shield* equipped_shield = nullptr;

	BodyPart queued_hit_part = BodyPart::NONE;
	float queued_damage = 0.0f;

	character(std::string n, int hp, int bdef, int b_ch, int c_ch, int d_ch, int rdh, Texture2D g, Vector3 pos, float rot);
	character(std::string n, const limbs_struct& l, int bdef, int b_ch, int c_ch, int d_ch, int rdh, Texture2D g, Vector3 pos, float rot);
	virtual ~character() = default;

	virtual bool is_dead() = 0;

	virtual void process_turn_start_effects(gamestate* gs);
	virtual int take_damage(int dmg_amount, const character* attacker, bool is_crit, bool is_guard, BodyPart hit_part, gamestate* gs, Vector3 impact_pos = { 0.0f, 0.0f, 0.0f });

	std::pair<int, bool> calculate_dmg(int limb_damage);

	std::string get_name() { return name; }

	int get_health() { return current_health; }
	int get_max_health() { return max_health; }
	int get_defense() { return base_defense; }
	int get_block_chance() { return block_chance; }
	int get_crit_chance() { return crit_chance; }
	int get_dodge_chance() { return dodge_chance; }
	int get_reduced_head_damage() { return reduced_head_damage; }

	Vector3 get_visual_position() const { return this->position; }

	void set_max_health(int val) { max_health = val; }
	void set_health(int val) { current_health = val; }

	Vector3 get_position() const { return position; }
	void set_position(Vector3 new_pos) { position = new_pos; }

	bool get_has_head() const { return limbs.head.is_intact; }
	bool get_has_left_arm() const { return limbs.left_arm.is_intact; }
	bool get_has_right_arm() const { return limbs.right_arm.is_intact; }
	bool get_has_left_leg() const { return limbs.left_leg.is_intact; }
	bool get_has_right_leg() const { return limbs.right_leg.is_intact; }

	void set_has_head(bool state) { limbs.head.is_intact = state; }
	void set_has_left_arm(bool state) { limbs.left_arm.is_intact = state; }
	void set_has_right_arm(bool state) { limbs.right_arm.is_intact = state; }
	void set_has_left_leg(bool state) { limbs.left_leg.is_intact = state; }
	void set_has_right_leg(bool state) { limbs.right_leg.is_intact = state; }

	void recalculate_max_health();
};
