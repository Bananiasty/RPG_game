#pragma once
#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <utility>
#include "struct.h"
#include "raylib.h"
#include "inventory_class.h"
#include "gamestates.h"



class gamestate;
class exploration;

class character
{

friend class item;
protected:
	std::string name;
	int base_defense, base_damage;
	int	block_chance, crit_chance, dodge_chance;
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

	helm* equipped_helm = nullptr;
	vest* equipped_vest = nullptr;
	gauntlets* equipped_gauntlets = nullptr;
	boots* equipped_boots = nullptr;
	weapon* equipped_weapon = nullptr;
	shield* equipped_shield = nullptr;
		/*equipped_accessory_1 = nullptr;
		equipped_accessory_2 = nullptr;
		equipped_accessory_3 = nullptr;*/


	
	BodyPart queued_hit_part = BodyPart::NONE;
	float queued_damage = 0.0;


	character(std::string n, const limbs_struct& l, int bdef, int b_ch, int c_ch, int d_ch, int rdh,Texture2D g, Vector3 pos, float rot);

	virtual bool is_dead() = 0;

	int take_damage(int dmg_amount, const character* attacker, bool is_crit, bool is_guard, BodyPart hit_part, gamestate* gs, Vector3 impact_pos = { 0.0f, 0.0f, 0.0f });

	std::pair<int, bool> calculate_dmg(int limb_damage);

	std::string get_name() { return name; }
	
	int get_health() { return current_health; }
	int get_max_health() { return max_health; }
	//int get_max_mana() { return max_mana; }
	int get_damage() { return base_damage; }
	int get_defense() { return base_defense; }
	int get_block_chance() { return block_chance; }
	int get_crit_chance() { return crit_chance; }
	int get_dodge_chance() { return dodge_chance; }
	int get_reduced_head_damage() { return reduced_head_damage; }

	Vector3 get_visual_position() const{return this->position;}


	void set_max_health(int val) { max_health = val; }
	void set_health(int val) { current_health = val; }
	//void set_max_mana(int val) { max_mana = val; }

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

class player :public character
{
private:
	int gold;
	int xp;
	int level;
	int xp_to_level_up=1;

	
public:
	player(std::string n, const limbs_struct& l, int bdef, int b_ch, int c_ch, int d_ch, int rdh, int xp, int level, Texture2D grafika, Vector3 pos, float rot);

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

	void take_all_loot(drop_object* c);
	void take_item(drop_object* c, item* it);

	void grant_xp();

	void sort_bag();

	void check_level_up();
};

class enemy : public character
{
private:
	std::string intro_text;
	int difficulty;
	int id_number;
	int room_id_number;
	float rotation = 0.0f;
	bool moving = false;
	bool is_alerted = false;

	Vector3 target_tile = { 0.0f, 0.0f, 0.0f };
	Vector3 forward = { 0.0f, 0.0f, 1.0f };

	int last_frame_index = 0;
	BodyPart current_hovered_part = BodyPart::NONE; 

	bool can_survive_without_head = false;

public:
	std::vector<item*> loot;

	virtual ~enemy() = default;

	bool is_dead() override;
	

	virtual BodyPart calculate_hovered_body_part(Vector3 drawPos, float targetWidth, float targetHeight, Camera3D camera);

	BodyPart get_hovered_body_part() const { return current_hovered_part; }
	void set_hovered_body_part(BodyPart part) { current_hovered_part = part; }

	virtual SpriteRenderParams get_render_params(Texture2D texture) const = 0;
	virtual int execute_ai_turn(character& target);


	virtual enemy* clone() const = 0;

	BodyPart select_random_target_part(const character& target);
	float get_rotation() const { return rotation; }

	int get_last_frame() const { return last_frame_index; }
	void set_last_frame(int frame) { last_frame_index = frame; }

	void set_forward(Vector3 f) { this->forward = f; }
	Vector3 get_forward() const { return this->forward; }

	bool is_moving() const { return moving; }
	void set_is_moving(bool state) { moving = state; }

	bool get_is_alerted() const { return is_alerted; }
	void set_is_alerted(bool state) { is_alerted = state; }
	

	void set_can_survive_without_head(bool status) { can_survive_without_head = status; }

	void set_target_tile(Vector3 tile) { target_tile = tile; }
	
	Vector3 get_target_tile() const { return target_tile; }

	Texture2D get_texture() const { return grafika; }

	std::string get_intro_text() { return intro_text; }

	int GetAdjustedSideLimit(int baseSideLimit, int frameIndex);

	enemy(const enemy_config& config);

	int get_dif() { return difficulty; }
	int get_id() { return id_number; }
	int get_room_id() { return room_id_number; }
};

class ghoul : public enemy
{
public:
	ghoul(const enemy_config& config) : enemy(config) 
	{
		limbs.left_arm.is_intact = false;
		limbs.left_arm.hp = 0;

		limbs.right_arm.can_attack = true;
		limbs.right_arm.damage = 25;

		limbs.head.can_attack = true;
		limbs.head.damage = 7;
	}
	
	SpriteRenderParams get_render_params(Texture2D texture) const override
	{
		constexpr float GHOUL_Y_OFFSET = 0.15f;

		float frameWidth = (float)texture.width / 8.0f;
		float frameHeight = (float)texture.height / 4.0f;
		float targetHeight = 2.0f;
		float targetWidth = targetHeight * (frameWidth / frameHeight);

		Vector3 pos = get_position();
		Vector3 drawPos = { pos.x, pos.y + GHOUL_Y_OFFSET, pos.z };

		return { drawPos, targetWidth, targetHeight, frameWidth, frameHeight };
	}

	ghoul* clone() const override
	{
		return new ghoul(*this);
	}

	int execute_ai_turn(character& target) override;
};

