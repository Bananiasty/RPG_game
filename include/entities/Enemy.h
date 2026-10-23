#pragma once
#include <string>
#include <vector>
#include "entities/Character.h"
#include "entities/EnemyConfig.h"
#include "struct.h"

class item;

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
	BodyPart attack_part_1 = BodyPart::NONE;
	BodyPart attack_part_2 = BodyPart::NONE;
	std::vector<item*> loot;

	virtual ~enemy() = default;

	bool is_dead() override;

	virtual BodyPart calculate_hovered_body_part(Vector3 drawPos, float targetWidth, float targetHeight, Camera3D camera) = 0;

	BodyPart get_hovered_body_part() const { return current_hovered_part; }
	void set_hovered_body_part(BodyPart part) { current_hovered_part = part; }

	virtual SpriteRenderParams get_render_params(Texture2D texture) const = 0;
	virtual int execute_ai_turn(character& target) = 0;
	virtual std::vector<BodyPart> decide_attacks() { return {}; }
	virtual int perform_strike(BodyPart attacking_part, character& target);

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
