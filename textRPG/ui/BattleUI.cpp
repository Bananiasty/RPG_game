#include "ui/BattleUI.h"
#include "graphics.h"
#include "gamestates.h"
#include "entities/Player.h"
#include "entities/Enemy.h"
#include "raylib.h"
#include "raymath.h"
#include "rlgl.h"
#include <string>
#include <cmath>
#include <algorithm>

BodyStatusUI body_UI;

int BodyStatusUI::get_status_code(const limb& l)
{
	if (!l.is_intact) return 3;               // Odcięta
	if (l.severely_wounded || l.hp <= 0) return 2; // Zniszczona 
	if (l.limb_wounded || l.hp < (l.max_hp * 0.8f)) return 1;   // Ranna
	return 0;                                 // Zdrowa
}

void BodyStatusUI::init()
{
	tex_silhouette = LoadTexture("graphics/UI/man_silhouette.png");
	tex_mask = LoadTexture("graphics/UI/man_body_mask.png");

	shader = LoadShader(nullptr, "graphics/shaders/bodystatus.fs");

	loc_mask = GetShaderLocation(shader, "maskTexture");

	loc_head = GetShaderLocation(shader, "head_status");
	loc_torso = GetShaderLocation(shader, "torso_status");
	loc_left_arm = GetShaderLocation(shader, "left_arm_status");
	loc_right_arm = GetShaderLocation(shader, "right_arm_status");
	loc_left_leg = GetShaderLocation(shader, "left_leg_status");
	loc_right_leg = GetShaderLocation(shader, "right_leg_status");

	int mask_texture_slot = 1;
	SetShaderValue(shader, loc_mask, &mask_texture_slot, SHADER_UNIFORM_INT);
}

void BodyStatusUI::draw(const player& p, Vector2 position, float scale)
{
	int head_st = get_status_code(p.limbs.head);
	int torso_st = get_status_code(p.limbs.torso);
	int larm_st = get_status_code(p.limbs.left_arm);
	int rarm_st = get_status_code(p.limbs.right_arm);
	int lleg_st = get_status_code(p.limbs.left_leg);
	int rleg_st = get_status_code(p.limbs.right_leg);

	SetShaderValue(shader, loc_head, &head_st, SHADER_UNIFORM_INT);
	SetShaderValue(shader, loc_torso, &torso_st, SHADER_UNIFORM_INT);
	SetShaderValue(shader, loc_left_arm, &larm_st, SHADER_UNIFORM_INT);
	SetShaderValue(shader, loc_right_arm, &rarm_st, SHADER_UNIFORM_INT);
	SetShaderValue(shader, loc_left_leg, &lleg_st, SHADER_UNIFORM_INT);
	SetShaderValue(shader, loc_right_leg, &rleg_st, SHADER_UNIFORM_INT);

	BeginShaderMode(shader);

	rlActiveTextureSlot(1);
	rlEnableTexture(tex_mask.id);

	rlActiveTextureSlot(0);
	DrawTextureEx(tex_silhouette, position, 0.0f, scale, WHITE);

	rlDrawRenderBatchActive();

	rlActiveTextureSlot(1);
	rlDisableTexture();

	rlActiveTextureSlot(0);

	EndShaderMode();
}

void BodyStatusUI::cleanup()
{
	UnloadTexture(tex_silhouette);
	UnloadTexture(tex_mask);
	UnloadShader(shader);
}

extern Font cabin_sketch_font;

void draw_battle_ui(battle* fight)
{
	Vector2 mouse = GetVirtualMousePosition();

	float p_hp_width = ((float)fight->p_ref.get_health() / fight->p_ref.get_max_health()) * 400.0f;

	Vector2 body_pos = { GAME_WIDTH * 0.85f, GAME_HEIGHT * 0.60f };
	body_UI.draw(fight->p_ref, body_pos, 2.0f);

	// PASEK ZDROWIA GRACZA
	float player_hp_x = 100.0f;
	float player_hp_y = GAME_HEIGHT * 0.80f;

	DrawTextEx(cabin_sketch_font, TextFormat("HP Bohatera: %d", fight->p_ref.get_health()), { player_hp_x, player_hp_y - 35.0f }, 28, 2, MAROON);
	DrawRectangle((int)player_hp_x, (int)player_hp_y, 400, 35, DARKGRAY);
	DrawRectangle((int)player_hp_x, (int)player_hp_y, (int)p_hp_width, 35, RED);

	Rectangle battle_menu_button = { GAME_WIDTH * 0.40f, GAME_HEIGHT * 0.82f, 220.0f, 60.0f };
	float button_spacing = battle_menu_button.width + 20.0f;

	bool is_attack_hovered = CheckCollisionPointRec(mouse, battle_menu_button);
	Color attack_btn_color = is_attack_hovered ? GRAY : LIGHTGRAY;

	DrawRectangleRec(battle_menu_button, attack_btn_color);
	DrawRectangleLinesEx(battle_menu_button, 2.0f, DARKGRAY);

	Vector2 attack_text_size = MeasureTextEx(cabin_sketch_font, "Attack", 40, 1);
	Vector2 attack_text_pos = {
		battle_menu_button.x + (battle_menu_button.width - attack_text_size.x) * 0.5f,
		battle_menu_button.y + (battle_menu_button.height - attack_text_size.y) * 0.5f
	};
	DrawTextEx(cabin_sketch_font, "Attack", attack_text_pos, 40, 1, BLACK);

	if (is_attack_hovered && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
	{
		fight->attack_clicked = true;
	}

	// PRZYCISK GUARD
	Rectangle guard_button_rec = { battle_menu_button.x + button_spacing, battle_menu_button.y, battle_menu_button.width, battle_menu_button.height };

	bool is_guard_hovered = CheckCollisionPointRec(mouse, guard_button_rec);
	Color guard_btn_color = is_guard_hovered ? GRAY : LIGHTGRAY;

	DrawRectangleRec(guard_button_rec, guard_btn_color);
	DrawRectangleLinesEx(guard_button_rec, 2.0f, DARKGRAY);

	Vector2 guard_text_size = MeasureTextEx(cabin_sketch_font, "Guard", 40, 1);
	Vector2 guard_text_pos = {
		guard_button_rec.x + (guard_button_rec.width - guard_text_size.x) * 0.5f,
		guard_button_rec.y + (guard_button_rec.height - guard_text_size.y) * 0.5f
	};
	DrawTextEx(cabin_sketch_font, "Guard", guard_text_pos, 40, 1, BLACK);

	if (is_guard_hovered && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
	{
		if (fight->player_cooldown <= 0)
		{
			fight->guard_clicked = true;
			fight->player_cooldown = 1.0;
		}
	}

	// STRZALKI (< i >)
	float arrow_box_size = 90.0f;
	float arrow_y = (GAME_HEIGHT - arrow_box_size) * 0.45f;
	float pulse = (sinf((float)GetTime() * 8.0f) + 1.0f) * 0.5f;

	bool can_left = fight->can_turn_left();
	bool can_right = fight->can_turn_right();
	bool has_left = fight->has_enemy_on_left();
	bool has_right = fight->has_enemy_on_right();

	Rectangle left_hitbox = { 35.0f, arrow_y, arrow_box_size, arrow_box_size };
	Rectangle right_hitbox = { GAME_WIDTH - arrow_box_size - 35.0f, arrow_y, arrow_box_size, arrow_box_size };

	// --- LEWA STRZALKA ---
	if (can_left)
	{
		bool is_left_hovered = CheckCollisionPointRec(mouse, left_hitbox);
		Color arrow_col_left = is_left_hovered ? (has_left ? YELLOW : WHITE)
		                                       : (has_left ? Fade(RED, 0.75f + 0.25f * pulse) : Fade(LIGHTGRAY, 0.70f));

		Vector2 arrow_size_l = MeasureTextEx(cabin_sketch_font, "<", 90.0f, 1.0f);
		Vector2 arrow_pos_l = {
			left_hitbox.x + (arrow_box_size - arrow_size_l.x) * 0.5f,
			left_hitbox.y + (arrow_box_size - arrow_size_l.y) * 0.5f
		};

		DrawTextEx(cabin_sketch_font, "<", { arrow_pos_l.x + 2.0f, arrow_pos_l.y + 2.0f }, 90.0f, 1.0f, Fade(BLACK, 0.85f));
		DrawTextEx(cabin_sketch_font, "<", arrow_pos_l, 90.0f, 1.0f, arrow_col_left);

		if (is_left_hovered && IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && !fight->waiting_for_enemy && !fight->click_cooldown)
		{
			fight->turn_camera_left();
		}
	}

	// --- PRAWA STRZALKA ---
	if (can_right)
	{
		bool is_right_hovered = CheckCollisionPointRec(mouse, right_hitbox);
		Color arrow_col_right = is_right_hovered ? (has_right ? YELLOW : WHITE)
		                                         : (has_right ? Fade(RED, 0.75f + 0.25f * pulse) : Fade(LIGHTGRAY, 0.70f));

		Vector2 arrow_size_r = MeasureTextEx(cabin_sketch_font, ">", 90.0f, 1.0f);
		Vector2 arrow_pos_r = {
			right_hitbox.x + (arrow_box_size - arrow_size_r.x) * 0.5f,
			right_hitbox.y + (arrow_box_size - arrow_size_r.y) * 0.5f
		};

		DrawTextEx(cabin_sketch_font, ">", { arrow_pos_r.x + 2.0f, arrow_pos_r.y + 2.0f }, 90.0f, 1.0f, Fade(BLACK, 0.85f));
		DrawTextEx(cabin_sketch_font, ">", arrow_pos_r, 90.0f, 1.0f, arrow_col_right);

		if (is_right_hovered && IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && !fight->waiting_for_enemy && !fight->click_cooldown)
		{
			fight->turn_camera_right();
		}
	}

	// WYKRZYKNIKI
	enum class CornerAlert {
		NONE_ALERT,
		YELLOW_ALERT,
		RED_ALERT,
		PULSE_ALERT
	};
	CornerAlert left_alert = CornerAlert::NONE_ALERT;
	CornerAlert right_alert = CornerAlert::NONE_ALERT;

	Vector3 player_right = Vector3Normalize(Vector3CrossProduct(fight->combat_facing_dir, { 0.0f, 1.0f, 0.0f }));

	auto upgrade_alert = [](CornerAlert current, CornerAlert next_alert) -> CornerAlert {
		if (current == CornerAlert::PULSE_ALERT || next_alert == CornerAlert::PULSE_ALERT) return CornerAlert::PULSE_ALERT;
		if (current == CornerAlert::RED_ALERT || next_alert == CornerAlert::RED_ALERT) return CornerAlert::RED_ALERT;
		if (current == CornerAlert::YELLOW_ALERT || next_alert == CornerAlert::YELLOW_ALERT) return CornerAlert::YELLOW_ALERT;
		return CornerAlert::NONE_ALERT;
	};

	for (const auto& rein : fight->incoming_reinforcements)
	{
		if (rein.enemy_ptr == nullptr || rein.enemy_ptr->is_dead()) continue;

		Vector3 to_e = Vector3Subtract(rein.enemy_ptr->get_position(), fight->combat_player_pos);
		to_e.y = 0.0f;
		float dist = Vector3Length(to_e);
		Vector3 dir = (dist > 0.01f) ? Vector3Scale(to_e, 1.0f / dist) : fight->combat_facing_dir;

		float dot_front = Vector3DotProduct(dir, fight->combat_facing_dir);
		if (dot_front > 0.35f) continue;

		float dot_side = Vector3DotProduct(to_e, player_right);
		bool is_left = (dot_side < 0.0f);
		if (dot_side == 0.0f) is_left = rein.is_approaching_from_left;

		CornerAlert alert = (rein.rounds_to_arrive >= 2) ? CornerAlert::YELLOW_ALERT : CornerAlert::RED_ALERT;
		if (is_left) left_alert = upgrade_alert(left_alert, alert);
		else right_alert = upgrade_alert(right_alert, alert);
	}

	for (auto it = fight->unconfirmed_reinforcements.begin(); it != fight->unconfirmed_reinforcements.end(); )
	{
		enemy* e = *it;
		if (e == nullptr || e->is_dead())
		{
			it = fight->unconfirmed_reinforcements.erase(it);
			continue;
		}

		Vector3 to_e = Vector3Subtract(e->get_position(), fight->combat_player_pos);
		to_e.y = 0.0f;
		float dist = Vector3Length(to_e);
		Vector3 dir = (dist > 0.01f) ? Vector3Scale(to_e, 1.0f / dist) : fight->combat_facing_dir;

		float dot_front = Vector3DotProduct(dir, fight->combat_facing_dir);
		bool is_in_combat_with_enemy = (dot_front > 0.35f) || (fight->get_current_target() == e);

		if (is_in_combat_with_enemy)
		{
			it = fight->unconfirmed_reinforcements.erase(it);
			continue;
		}

		float dot_side = Vector3DotProduct(to_e, player_right);
		bool is_left = (dot_side < 0.0f);
		if (dot_side == 0.0f) is_left = fight->can_turn_left();

		if (is_left) left_alert = upgrade_alert(left_alert, CornerAlert::PULSE_ALERT);
		else right_alert = upgrade_alert(right_alert, CornerAlert::PULSE_ALERT);

		++it;
	}

	auto draw_corner_alert = [&](CornerAlert alert, bool is_left) {
		if (alert == CornerAlert::NONE_ALERT) return;

		float base_size = 80.0f;
		Vector2 def_size = MeasureTextEx(cabin_sketch_font, "!", base_size, 1.0f);

		float pos_x = is_left ? 50.0f : (GAME_WIDTH - 50.0f - def_size.x);
		float pos_y = 40.0f;

		if (alert == CornerAlert::YELLOW_ALERT)
		{
			DrawTextEx(cabin_sketch_font, "!", { pos_x + 2.0f, pos_y + 2.0f }, base_size, 1.0f, Fade(BLACK, 0.85f));
			DrawTextEx(cabin_sketch_font, "!", { pos_x, pos_y }, base_size, 1.0f, YELLOW);
		}
		else if (alert == CornerAlert::RED_ALERT)
		{
			DrawTextEx(cabin_sketch_font, "!", { pos_x + 2.0f, pos_y + 2.0f }, base_size, 1.0f, Fade(BLACK, 0.85f));
			DrawTextEx(cabin_sketch_font, "!", { pos_x, pos_y }, base_size, 1.0f, RED);
		}
		else if (alert == CornerAlert::PULSE_ALERT)
		{
			float pulse_t = (sinf((float)GetTime() * 10.0f) + 1.0f) * 0.5f;
			float cur_font_size = base_size * (1.0f + 0.28f * pulse_t);
			Vector2 cur_size = MeasureTextEx(cabin_sketch_font, "!", cur_font_size, 1.0f);

			Vector2 center = { pos_x + def_size.x * 0.5f, pos_y + def_size.y * 0.5f };
			Vector2 draw_p = { center.x - cur_size.x * 0.5f, center.y - cur_size.y * 0.5f };

			Color pulse_col = ColorAlphaBlend(RED, YELLOW, Fade(WHITE, pulse_t * 0.70f));

			DrawTextEx(cabin_sketch_font, "!", { draw_p.x + 3.0f, draw_p.y + 3.0f }, cur_font_size, 1.0f, Fade(BLACK, 0.90f));
			DrawTextEx(cabin_sketch_font, "!", draw_p, cur_font_size, 1.0f, pulse_col);
		}
	};

	draw_corner_alert(left_alert, true);
	draw_corner_alert(right_alert, false);

	// PRZECIWNICY OCZEKUJACY W KOLEJCE
	if (!fight->queued_enemies.empty())
	{
		float queue_box_w = 520.0f;
		float queue_box_h = 38.0f;
		float queue_box_x = (GAME_WIDTH - queue_box_w) * 0.5f;
		float queue_box_y = 30.0f;

		for (size_t i = 0; i < fight->queued_enemies.size(); ++i)
		{
			enemy* q_enemy = fight->queued_enemies[i];
			if (q_enemy == nullptr) continue;

			float current_y = queue_box_y + (i * (queue_box_h + 8.0f));

			DrawRectangle((int)queue_box_x, (int)current_y, (int)queue_box_w, (int)queue_box_h, Fade(BLACK, 0.8f));
			DrawRectangleLines((int)queue_box_x, (int)current_y, (int)queue_box_w, (int)queue_box_h, ORANGE);

			const char* queue_text = TextFormat("%s waiting in line behind opponent", q_enemy->get_name().c_str());
			DrawTextEx(cabin_sketch_font, queue_text, { queue_box_x + 16.0f, current_y + 6.0f }, 22, 1, ORANGE);
		}
	}
}
