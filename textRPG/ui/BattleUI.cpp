#include "ui/BattleUI.h"
#include "graphics.h"
#include "gamestates.h"
#include "entities/Player.h"
#include "entities/Enemy.h"
#include "raylib.h"
#include "rlgl.h"
#include <string>

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

	// LICZNIK NADCHODZACYCH WZMOCNIEN (REINFORCEMENTS TIMER) - PO PRAWEJ STRONIE, BEZ CZERWONEGO KONTURU
	if (!fight->incoming_reinforcements.empty())
	{
		float rein_box_w = 90.0f;
		float rein_box_h = 52.0f;
		float rein_box_x = GAME_WIDTH - rein_box_w - 60.0f;
		float rein_box_y = 50.0f;

		for (size_t i = 0; i < fight->incoming_reinforcements.size(); ++i)
		{
			const auto& rein = fight->incoming_reinforcements[i];
			if (rein.enemy_ptr == nullptr) continue;

			float current_y = rein_box_y + (i * (rein_box_h + 10.0f));

			const char* rein_text = TextFormat("! %d", rein.rounds_to_arrive);
			Vector2 text_size = MeasureTextEx(cabin_sketch_font, rein_text, 36, 1);
			Vector2 text_pos = {
				rein_box_x + (rein_box_w - text_size.x) * 0.5f,
				current_y + (rein_box_h - text_size.y) * 0.5f
			};

			DrawTextEx(cabin_sketch_font, rein_text, text_pos, 50, 1, YELLOW);
		}
	}

	// PRZECIWNICY OCZEKUJACY W KOLEJCE (W WASKIM KORYTARZU)
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
