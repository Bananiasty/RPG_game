#include <vector>
#include <algorithm>
#include "raylib.h"
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"
#include "raymath.h"
#include "rlgl.h"
#include "graphics.h"
#include "character.h"
#include "struct.h"
#include "inventory_class.h"
#include "textureManager.h"
#include <ranges>
#include <utility>
#include <map>	

extern Font cabin_sketch_font;
extern Font cabin_sketch_font_bold;
float screen_width = 0.0f;
float screen_height = 0.0f;


	

void draw_game_scene(exploration* exp)
{


	ClearBackground(BLACK);

	int viewPosLoc = GetShaderLocation(textures.fogShader, "viewPos");
	SetShaderValue(textures.fogShader, viewPosLoc, &exp->camera.position, SHADER_UNIFORM_VEC3);

	BeginMode3D(exp->camera);

	DrawExploration(exp); 

	EndMode3D();

	DrawHUD(exp);
	 

}


void draw_login_screen(const std::string& current_name, bool has_error)
{
	ClearBackground(BLACK);

	DrawText( "LOGOWANIE", 500, 100, 50, GOLD);
	DrawTextEx(cabin_sketch_font, "Wpisz swoj nickname (3-12 znakow, bez znakow specjalnych):", { 350, 180 }, 20, 2, GRAY);

	DrawRectangleLines(500, 230, 300, 40, LIGHTGRAY);
	DrawTextEx(cabin_sketch_font, current_name.c_str(), { 510, 240 }, 20, 3, RAYWHITE);

	DrawTextEx(cabin_sketch_font, "Wcisnij [ENTER] aby zatwierdzic", { 500, 300 }, 16, 2, DARKGRAY);

	if (has_error)
	{
		DrawTextEx(cabin_sketch_font, "BLAD: Nick zawiera znaki specjalne lub ma zla dlugosc!", { 250, 350 }, 18, 2, RED);
	}
}

struct RenderObject
{
	float distance_sqr = 0.0f;

	chest* chest_ptr = nullptr;
	enemy* enemy_ptr = nullptr;
};


std::pair<float, int> GetOutlineParams(BodyPart part, float frameHeight)
{
	switch (part)
	{
	case BodyPart::TORSO:     return { 0.0f * frameHeight, 0 };
	case BodyPart::HEAD:      return { 1.0f * frameHeight, 0 };
	case BodyPart::RIGHT_ARM: return { 2.0f * frameHeight, 1 };
	case BodyPart::LEFT_ARM:  return { 2.0f * frameHeight, 2 };
	case BodyPart::RIGHT_LEG: return { 3.0f * frameHeight, 1 };
	case BodyPart::LEFT_LEG:  return { 3.0f * frameHeight, 2 };
	default:                  return { 0.0f, 0 };
	}
}

void DrawOutlineBillboard(Camera3D camera, Texture2D texture, Shader shader, Rectangle rec, Vector3 pos, Vector2 size, int sideLimit)
{
	rlDisableDepthMask();

	float texSize[2] = { (float)texture.width, (float)texture.height };
	float color[4] = { 1.0f, 1.0f, 1.0f, 1.0f };

	SetShaderValue(shader, GetShaderLocation(shader, "textureSize"), texSize, SHADER_UNIFORM_VEC2);
	SetShaderValue(shader, GetShaderLocation(shader, "outlineColor"), color, SHADER_UNIFORM_VEC4);
	SetShaderValue(shader, GetShaderLocation(shader, "sideLimit"), &sideLimit, SHADER_UNIFORM_INT);

	SetTextureFilter(texture, TEXTURE_FILTER_BILINEAR);
	BeginShaderMode(shader);
	DrawBillboardRec(camera, texture, rec, pos, size, WHITE);
	EndShaderMode();
	SetTextureFilter(texture, TEXTURE_FILTER_POINT);

	rlEnableDepthMask();
}

void DrawExploration(exploration* exp)
{
	int promienWidzenia = 20;

	int playerGridX = (int)(exp->camera.position.x / 2.0f);
	int playerGridY = (int)(exp->camera.position.z / 2.0f);

	int startX = std::max(0, playerGridX - promienWidzenia);
	int endX = std::min(exp->szerokosc, playerGridX + promienWidzenia);
	int startY = std::max(0, playerGridY - promienWidzenia);
	int endY = std::min(exp->dlugosc, playerGridY + promienWidzenia);

	std::vector<RenderObject> object_to_draw;

	Vector3 enemyForward = { 0.0f, 0.0f, 1.0f };

	for (auto* c : exp->world_chests)
	{
		int chestGridX = (int)(c->position.x / 2.0f);
		int chestGridY = (int)(c->position.z / 2.0f);

		if (chestGridX >= startX && chestGridX < endX && chestGridY >= startY && chestGridY < endY)
		{
			float dx = c->position.x - exp->camera.position.x;
			float dy = c->position.y - exp->camera.position.y;
			float dz = c->position.z - exp->camera.position.z;

			RenderObject object;
			object.chest_ptr = c;
			object.distance_sqr = (dx * dx) + (dy * dy) + (dz * dz);
			object_to_draw.push_back(object);
		}
	}
	for (auto& [id, node] : exp->world_map)
	{
		if (node.enemy != nullptr && !node.enemy->is_dead())
		{
			Vector3 pos = node.enemy->get_position();
			int enemyGridX = (int)(pos.x / 2.0f);
			int enemyGridY = (int)(pos.z / 2.0f);

			if (enemyGridX >= startX && enemyGridX < endX &&
				enemyGridY >= startY && enemyGridY < endY)
			{
				float dx = pos.x - exp->camera.position.x;
				float dy = pos.y - exp->camera.position.y;
				float dz = pos.z - exp->camera.position.z;

				RenderObject object;
				object.enemy_ptr = node.enemy;
				object.distance_sqr = (dx * dx) + (dy * dy) + (dz * dz);
				object_to_draw.push_back(object);
			}
		}
	}
	std::sort(object_to_draw.begin(), object_to_draw.end(), [](const RenderObject& a, const RenderObject& b)
		{
			return a.distance_sqr > b.distance_sqr;
		});

	for (const auto& object : object_to_draw)
	{
		if (object.chest_ptr != nullptr)
		{
			auto* c = object.chest_ptr;
			if (c->enemy_ptr != nullptr)
			{
				DrawCube(c->position, 0.7f, 0.1f, 0.7f, RED);
				DrawCubeWires(c->position, 0.7f, 0.1f, 0.7f, MAROON);
			}
			else
			{
				DrawModel(objects.m_chest, c->position, 1.5f, WHITE);
				DrawCubeWires(c->position, 0.6f, 0.6f, 0.6f, DARKBROWN);
			}
		}

		else if (object.enemy_ptr != nullptr)
		{
			auto* enemy = object.enemy_ptr;
			int current_id = enemy->get_id();
			Vector3 pos = enemy->get_position();

			switch (current_id)
			{
			case 1:
			{
				auto params = enemy->get_render_params(textures.ghoul);

				int lastFrame = enemy->get_last_frame();
				int frameIndex = GetSpriteFrameIndex(pos, enemy->get_forward(), exp->camera.position, lastFrame);
				enemy->set_last_frame(frameIndex);

				BodyPart hoveredPart = enemy->get_hovered_body_part();
				const auto& limbs = enemy->get_limbs();

				SetShaderValue(textures.fogShader, GetShaderLocation(textures.fogShader, "viewPos"), &exp->camera.position, SHADER_UNIFORM_VEC3);

				int sideLimitLoc = GetShaderLocation(textures.fogShader, "sideLimit");

				BeginShaderMode(textures.fogShader);

				if (limbs.torso.is_intact)
				{
					int sideLimit = 0;
					SetShaderValue(textures.fogShader, sideLimitLoc, &sideLimit, SHADER_UNIFORM_INT);

					Rectangle rowSourceRec = { frameIndex * params.frameWidth, 0 * params.frameHeight, params.frameWidth, params.frameHeight };
					DrawBillboardRec(exp->camera, textures.ghoul, rowSourceRec, params.drawPos, { params.targetWidth, params.targetHeight }, WHITE);
					rlDrawRenderBatchActive();
				}

				if (limbs.head.is_intact)
				{
					int sideLimit = 0;
					SetShaderValue(textures.fogShader, sideLimitLoc, &sideLimit, SHADER_UNIFORM_INT);

					Rectangle rowSourceRec = { frameIndex * params.frameWidth, 1 * params.frameHeight, params.frameWidth, params.frameHeight };
					DrawBillboardRec(exp->camera, textures.ghoul, rowSourceRec, params.drawPos, { params.targetWidth, params.targetHeight }, WHITE);
					rlDrawRenderBatchActive();
				}

				if (limbs.left_arm.is_intact || limbs.right_arm.is_intact)
				{
					int sideLimit = 0;
					if (!limbs.left_arm.is_intact)      sideLimit = 1;
					else if (!limbs.right_arm.is_intact) sideLimit = 2;

					SetShaderValue(textures.fogShader, sideLimitLoc, &sideLimit, SHADER_UNIFORM_INT);

					Rectangle rowSourceRec = { frameIndex * params.frameWidth, 2 * params.frameHeight, params.frameWidth, params.frameHeight };
					DrawBillboardRec(exp->camera, textures.ghoul, rowSourceRec, params.drawPos, { params.targetWidth, params.targetHeight }, WHITE);
					rlDrawRenderBatchActive();
				}

				if (limbs.left_leg.is_intact || limbs.right_leg.is_intact)
				{
					int sideLimit = 0;
					if (!limbs.left_leg.is_intact)      sideLimit = 1;
					else if (!limbs.right_leg.is_intact) sideLimit = 2;

					SetShaderValue(textures.fogShader, sideLimitLoc, &sideLimit, SHADER_UNIFORM_INT);

					Rectangle rowSourceRec = { frameIndex * params.frameWidth, 3 * params.frameHeight, params.frameWidth, params.frameHeight };
					DrawBillboardRec(exp->camera, textures.ghoul, rowSourceRec, params.drawPos, { params.targetWidth, params.targetHeight }, WHITE);
					rlDrawRenderBatchActive();
				}
				int resetSide = 0;
				SetShaderValue(textures.fogShader, sideLimitLoc, &resetSide, SHADER_UNIFORM_INT);
				EndShaderMode();

				if (hoveredPart != BodyPart::NONE)
				{
					auto [rowY, sideLimit] = GetOutlineParams(hoveredPart, params.frameHeight);
					Rectangle outlineRec = { frameIndex * params.frameWidth, rowY, params.frameWidth, params.frameHeight };

					DrawOutlineBillboard(exp->camera, textures.ghoul, textures.outlineShader, outlineRec, params.drawPos, { params.targetWidth, params.targetHeight }, sideLimit);

				}
				break;
			}
			}
		}
	}




		for (int cx = startX; cx < endX; cx++)
		{
			for (int cy = startY; cy < endY; cy++)
			{
				if (exp->dungeon[cx][cy] == 2)
				{
					Vector3 pozycja_dolna = Vector3{ (float)cx * 2.0f, 0.0f, (float)cy * 2.0f };
					Vector3 pozycja_gorna = Vector3{ (float)cx * 2.0f, 2.0f, (float)cy * 2.0f };

					//ŒCIANA PO£UDNIOWA
					if (cy > 0 && exp->dungeon[cx][cy - 1] == 1)
					{
						DrawModelEx(objects.wall_tile, pozycja_dolna, Vector3{ 0.0f, 1.0f, 0.0f }, 0.0f, Vector3{ 1.0f, 1.0f, 1.0f }, WHITE);
						DrawModelEx(objects.wall_tile, pozycja_gorna, Vector3{ 0.0f, 1.0f, 0.0f }, 0.0f, Vector3{ 1.0f, 1.0f, 1.0f }, WHITE);
					}
					//ŒCIANA PÓ£NOCNA
					if (cy < exp->dlugosc - 1 && exp->dungeon[cx][cy + 1] == 1)
					{
						Vector3 p_dolna = Vector3{ pozycja_dolna.x - 2.0f, 0.0f, pozycja_dolna.z + 2.0f };
						Vector3 p_gorna = Vector3{ pozycja_gorna.x - 2.0f, 2.0f, pozycja_gorna.z + 2.0f };
						DrawModelEx(objects.wall_tile, p_dolna, Vector3{ 0.0f, 1.0f, 0.0f }, 180.0f, Vector3{ 1.0f, 1.0f, 1.0f }, WHITE);
						DrawModelEx(objects.wall_tile, p_gorna, Vector3{ 0.0f, 1.0f, 0.0f }, 180.0f, Vector3{ 1.0f, 1.0f, 1.0f }, WHITE);
					}
					//ŒCIANA WSCHODNIA
					if (cx > 0 && exp->dungeon[cx - 1][cy] == 1)
					{
						Vector3 p_dolna = Vector3{ p_dolna.x = pozycja_dolna.x - 2.0f, 0.0f, pozycja_dolna.z };
						Vector3 p_gorna = Vector3{ p_gorna.x = pozycja_gorna.x - 2.0f, 2.0f, pozycja_gorna.z };
						DrawModelEx(objects.wall_tile, p_dolna, Vector3{ 0.0f, 1.0f, 0.0f }, 90.0f, Vector3{ 1.0f, 1.0f, 1.0f }, WHITE);
						DrawModelEx(objects.wall_tile, p_gorna, Vector3{ 0.0f, 1.0f, 0.0f }, 90.0f, Vector3{ 1.0f, 1.0f, 1.0f }, WHITE);
					}
					//ŒCIANA ZACHODNIA
					if (cx < exp->szerokosc - 1 && exp->dungeon[cx + 1][cy] == 1)
					{
						Vector3 p_dolna = Vector3{ pozycja_dolna.x, 0.0f, pozycja_dolna.z + 2.0f };
						Vector3 p_gorna = Vector3{ pozycja_gorna.x, 2.0f, pozycja_gorna.z + 2.0f };
						DrawModelEx(objects.wall_tile, p_dolna, Vector3{ 0.0f, 1.0f, 0.0f }, 270.0f, Vector3{ 1.0f, 1.0f, 1.0f }, WHITE);
						DrawModelEx(objects.wall_tile, p_gorna, Vector3{ 0.0f, 1.0f, 0.0f }, 270.0f, Vector3{ 1.0f, 1.0f, 1.0f }, WHITE);
					}
				}
				if (exp->dungeon[cx][cy] == 1)
				{
					DrawModel(objects.floor_tile, Vector3{ (float)cx * 2.0f, 0.0f , (float)cy * 2.0f }, 1.0f, WHITE);
					DrawModel(objects.ceiling_tile, Vector3{ (float)cx * 2.0f, 4.0f , (float)cy * 2.0f }, 1.0f, WHITE);
				}
			}
		}

}


void draw_battle_ui(battle* fight)
{
	Vector2 mouse = GetVirtualMousePosition();

	std::string e = fight->get_enemy_name();

	float p_hp_width = ((float)fight->p_ref.get_health() / fight->p_ref.get_max_health()) * 400.0f;

	//PASEK ZDROWIA GRACZA
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

	//PRZYCISK GUARD
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
}
void DrawHUD(exploration* exp)
{


	/*DrawTextEx(arial_font, TextFormat("LvL %d", exp->bohater.get_level()), { 10, 530 }, 20, 2, WHITE);
	DrawTextEx(arial_font, TextFormat("XP: %d/%d", exp->bohater.get_xp(), exp->bohater.get_xp_to_level_up()), { 10, 500 }, 20, 2, WHITE);
	DrawRectangle(10, 600, 30, 100, DARKBLUE);
	float xp_height = (float)exp->bohater.get_xp() / exp->bohater.get_xp_to_level_up() * 100;
	DrawRectangle(10, 700 - xp_height, 30, xp_height, WHITE);*/
}




void draw_inventory_ui(player& p, inventory_state* inv)
{

	Texture2D inventory = textures.inventory_UI;
	Texture2D item_icon;

	Vector2 mouse_pos = GetVirtualMousePosition();
	Vector2 window_center = { GAME_WIDTH * 0.5 - inventory.width * 0.5, GAME_HEIGHT * 0.5 - inventory.height * 0.5 };

	Rectangle equipment_button = { window_center.x + inventory.width * 0.057f, window_center.y + inventory.height * 0.081f, 120, 120 };
	Rectangle item_button = { window_center.x + inventory.width * 0.181f, window_center.y + inventory.height * 0.081f, 120, 120 };
	Rectangle food_button = { window_center.x + inventory.width * 0.308f, window_center.y + inventory.height * 0.081f, 120, 120 };
	Rectangle books_button = { window_center.x + inventory.width * 0.437f, window_center.y + inventory.height * 0.081f, 120, 120 };

	const float SLOT_SIZE = 75.0f;

	const float HELM_OFFSET_X = 1039.5f;
	const float HELM_OFFSET_Y = 75.0f;
	Rectangle helm_slot = { window_center.x + HELM_OFFSET_X, window_center.y + HELM_OFFSET_Y, SLOT_SIZE, SLOT_SIZE };

	const float WEAPON_OFFSET_X = 919.5f;
	const float WEAPON_OFFSET_Y = 186.0f;
	Rectangle weapon_slot = { window_center.x + WEAPON_OFFSET_X, window_center.y + WEAPON_OFFSET_Y, SLOT_SIZE, SLOT_SIZE };

	const float SHIELD_OFFSET_X = 1159.5f;
	const float SHIELD_OFFSET_Y = 186.0f;
	Rectangle shield_slot = { window_center.x + SHIELD_OFFSET_X, window_center.y + SHIELD_OFFSET_Y, SLOT_SIZE, SLOT_SIZE };

	const float VEST_OFFSET_X = 1039.5f;
	const float VEST_OFFSET_Y = 229.5f;
	Rectangle vest_slot = { window_center.x + VEST_OFFSET_X, window_center.y + VEST_OFFSET_Y, SLOT_SIZE, SLOT_SIZE };

	const float GLOVES_OFFSET_X = 918.0f;
	const float GLOVES_OFFSET_Y = 342.0f;
	Rectangle gloves_slot = { window_center.x + GLOVES_OFFSET_X, window_center.y + GLOVES_OFFSET_Y, SLOT_SIZE, SLOT_SIZE };

	const float BOOTS_OFFSET_X = 1159.5f;
	const float BOOTS_OFFSET_Y = 342.0f;
	Rectangle boots_slot = { window_center.x + BOOTS_OFFSET_X, window_center.y + BOOTS_OFFSET_Y, SLOT_SIZE, SLOT_SIZE };

	const float ACCESSORY_1_OFFSET_X = 1047.0f;
	const float ACCESSORY_1_OFFSET_Y = 541.5f;
	Rectangle accessory_1 = { window_center.x + ACCESSORY_1_OFFSET_X, window_center.y + ACCESSORY_1_OFFSET_Y, SLOT_SIZE, SLOT_SIZE };

	const float ACCESSORY_2_OFFSET_X = 1158.0f;
	const float ACCESSORY_2_OFFSET_Y = 541.5f;
	Rectangle accessory_2 = { window_center.x + ACCESSORY_2_OFFSET_X, window_center.y + ACCESSORY_2_OFFSET_Y, SLOT_SIZE, SLOT_SIZE };

	const float ACCESSORY_3_OFFSET_X = 1269.0f;
	const float ACCESSORY_3_OFFSET_Y = 541.5f;
	Rectangle accessory_3 = { window_center.x + ACCESSORY_3_OFFSET_X, window_center.y + ACCESSORY_3_OFFSET_Y, SLOT_SIZE, SLOT_SIZE };

	DrawTextureEx(inventory, window_center, 0.0f, 1.0f, WHITE);

	if (p.equipped_helm != nullptr)
	{
		Texture2D* icon = p.equipped_helm->icon_texture;
		Rectangle source_rec = { 0.0f, 0.0f, (float)icon->width, (float)icon->height };
		DrawTexturePro(*icon, source_rec, helm_slot, { 0.0f, 0.0f }, 0.0f, WHITE);

		if (CheckCollisionPointRec(mouse_pos, helm_slot))
		{
			DrawRectangleLinesEx(helm_slot, 2.0f, YELLOW);
			if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
			{
				p.equipped_helm->use(&p);
			}
		}
	}

	if (p.equipped_weapon != nullptr)
	{
		Texture2D* icon = p.equipped_weapon->icon_texture;
		Rectangle source_rec = { 0.0f, 0.0f, (float)icon->width, (float)icon->height };
		DrawTexturePro(*icon, source_rec, weapon_slot, { 0.0f, 0.0f }, 0.0f, WHITE);

		if (CheckCollisionPointRec(mouse_pos, weapon_slot))
		{
			DrawRectangleLinesEx(weapon_slot, 2.0f, YELLOW);
			if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
			{
				p.equipped_weapon->use(&p);
			}
		}
	}

	if (p.equipped_shield != nullptr)
	{
		Texture2D* icon = p.equipped_shield->icon_texture;
		Rectangle source_rec = { 0.0f, 0.0f, (float)icon->width, (float)icon->height };
		DrawTexturePro(*icon, source_rec, shield_slot, { 0.0f, 0.0f }, 0.0f, WHITE);

		if (CheckCollisionPointRec(mouse_pos, shield_slot))
		{
			DrawRectangleLinesEx(shield_slot, 2.0f, YELLOW);
			if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
			{
				p.equipped_shield->use(&p);
			}
		}
	}


	if (p.equipped_vest != nullptr)
	{
		Texture2D* icon = p.equipped_vest->icon_texture;
		Rectangle source_rec = { 0.0f, 0.0f, (float)icon->width, (float)icon->height };
		DrawTexturePro(*icon, source_rec, vest_slot, { 0.0f, 0.0f }, 0.0f, WHITE);

		if (CheckCollisionPointRec(mouse_pos, vest_slot))
		{
			DrawRectangleLinesEx(vest_slot, 2.0f, YELLOW);
			if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
			{
				p.equipped_vest->use(&p);
			}
		}
	}

	if (p.equipped_gauntlets != nullptr)
	{
		Texture2D* icon = p.equipped_gauntlets->icon_texture;
		Rectangle source_rec = { 0.0f, 0.0f, (float)icon->width, (float)icon->height };
		DrawTexturePro(*icon, source_rec, gloves_slot, { 0.0f, 0.0f }, 0.0f, WHITE);

		if (CheckCollisionPointRec(mouse_pos, gloves_slot))
		{
			DrawRectangleLinesEx(gloves_slot, 2.0f, YELLOW);
			if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
			{
				p.equipped_gauntlets->use(&p);
			}
		}
	}

	if (p.equipped_boots != nullptr)
	{
		Texture2D* icon = p.equipped_boots->icon_texture;
		Rectangle source_rec = { 0.0f, 0.0f, (float)icon->width, (float)icon->height };
		DrawTexturePro(*icon, source_rec, boots_slot, { 0.0f, 0.0f }, 0.0f, WHITE);

		if (CheckCollisionPointRec(mouse_pos, boots_slot))
		{
			DrawRectangleLinesEx(boots_slot, 2.0f, YELLOW);
			if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
			{
				p.equipped_boots->use(&p);
			}
		}
	}

	//// --- 7. AKCESORIUM 1 ---
	//if (p.equipped_accessory_1 != nullptr)
	//{
	//	Texture2D* icon = p.equipped_accessory_1->icon_texture;
	//	Rectangle source_rec = { 0.0f, 0.0f, (float)icon->width, (float)icon->height };
	//	DrawTexturePro(*icon, source_rec, accessory_1, { 0.0f, 0.0f }, 0.0f, WHITE);

	//	if (CheckCollisionPointRec(mouse_pos, accessory_1))
	//	{
	//		DrawRectangleLinesEx(accessory_1, 2.0f, YELLOW);
	//		if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
	//		{
	//			p.equipped_accessory_1->use(&p);
	//		}
	//	}
	//}

	//// --- 8. AKCESORIUM 2 ---
	//if (p.equipped_accessory_2 != nullptr)
	//{
	//	Texture2D* icon = p.equipped_accessory_2->icon_texture;
	//	Rectangle source_rec = { 0.0f, 0.0f, (float)icon->width, (float)icon->height };
	//	DrawTexturePro(*icon, source_rec, accessory_2, { 0.0f, 0.0f }, 0.0f, WHITE);

	//	if (CheckCollisionPointRec(mouse_pos, accessory_2))
	//	{
	//		DrawRectangleLinesEx(accessory_2, 2.0f, YELLOW);
	//		if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
	//		{
	//			p.equipped_accessory_2->use(&p);
	//		}
	//	}
	//}

	//// --- 9. AKCESORIUM 3 ---
	//if (p.equipped_accessory_3 != nullptr)
	//{
	//	Texture2D* icon = p.equipped_accessory_3->icon_texture;
	//	Rectangle source_rec = { 0.0f, 0.0f, (float)icon->width, (float)icon->height };
	//	DrawTexturePro(*icon, source_rec, accessory_3, { 0.0f, 0.0f }, 0.0f, WHITE);

	//	if (CheckCollisionPointRec(mouse_pos, accessory_3))
	//	{
	//		DrawRectangleLinesEx(accessory_3, 2.0f, YELLOW);
	//		if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
	//		{
	//			p.equipped_accessory_3->use(&p);
	//		}
	//	}
	//}

	bool equipment_button_isHovered = CheckCollisionPointRec(mouse_pos, equipment_button);
	bool item_button_isHovered = CheckCollisionPointRec(mouse_pos, item_button);
	bool food_button_isHovered = CheckCollisionPointRec(mouse_pos, food_button);
	bool books_button_isHovered = CheckCollisionPointRec(mouse_pos, books_button);


	p.sort_bag();



	if (equipment_button_isHovered)
	{
		DrawRectangleLinesEx(equipment_button, 5, WHITE);
		if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
		{
			inv->equipment_tab = true;
			inv->items_tab = false;
			inv->food_tab = false;
			inv->books_tab = false;
		}
	}
	if (item_button_isHovered)
	{
		DrawRectangleLinesEx(item_button, 5, WHITE);
		if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
		{
			inv->equipment_tab = false;
			inv->items_tab = true;
			inv->food_tab = false;
			inv->books_tab = false;
		}
	}
	if (food_button_isHovered)
	{
		DrawRectangleLinesEx(food_button, 5, WHITE);
		if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
		{
			inv->equipment_tab = false;
			inv->items_tab = false;
			inv->food_tab = true;
			inv->books_tab = false;
		}
	}
	if (books_button_isHovered)
	{
		DrawRectangleLinesEx(books_button, 5, WHITE);
		if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
		{
			inv->equipment_tab = false;
			inv->items_tab = false;
			inv->food_tab = false;
			inv->books_tab = true;
		}
	}
	auto& all_items = p.bag->items;

	auto& equipment = p.equipment->items;
	auto& other_items = p.inv_items->items;
	auto& food = p.food->items;
	auto& books = p.books->items;

	int startX = 80;
	int startY = 250;
	int btnWidth = 60;
	int btnHeight = 60;
	int bottom_padding = 15;
	int right_padding = 140;

	bool is_item_hovered;
	if (inv->equipment_tab == true)
	{
		int renderIndex = 0;
		for (int i = 0; i < equipment.size(); i++)
		{
			if (equipment[i]->is_equipped()) continue;

			item_icon = *(equipment[i]->icon_texture);

			int col = renderIndex / 8;
			int row = renderIndex % 8;
			Rectangle itemRect = {
				window_center.x + (float)(startX + col * (btnWidth + right_padding)),
				window_center.y + (float)(startY + row * (btnHeight + bottom_padding)),
				(float)btnWidth,
				(float)btnHeight
			};
			is_item_hovered = CheckCollisionPointRec(mouse_pos, itemRect);

			if (is_item_hovered)
			{
				if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
				{
					equipment[i]->use(&p);
					p.sort_bag();
					break;
				}
			}
			if (!equipment[i]->is_equipped())
			{
				if (item_icon.id > 0)
				{
					float scale = (float)(btnHeight - 10) / item_icon.height;
					DrawTextureEx(item_icon, { itemRect.x + 5, itemRect.y + 5 }, 0.0f, scale, WHITE);
				}

				if (is_item_hovered)
				{
					std::string label = equipment[i]->get_name();
					DrawRectangle(itemRect.x + btnWidth, itemRect.y, 180, 50, DARKBLUE);
					DrawTextEx(cabin_sketch_font, label.c_str(), { itemRect.x + 50, itemRect.y }, 36, 1, WHITE);
				}
			}
			renderIndex++;
		}
	}
	else if (inv->items_tab == true)
	{
		for (int i = 0; i < other_items.size(); i++)
		{
			item_icon = *(other_items[i]->icon_texture);

			int row = i / 5;
			int col = i % 5;
			Rectangle itemRect = {
				window_center.x + (float)(startX + col * (btnWidth + right_padding)),
				window_center.y + (float)(startY + row * (btnHeight + bottom_padding)),
				(float)btnWidth,
				(float)btnHeight
			};

			bool is_item_hovered = CheckCollisionPointRec(mouse_pos, itemRect);

			if (is_item_hovered && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
			{
				other_items[i]->use(&p);
				p.sort_bag();
				break;
			}

			if (item_icon.id > 0)
			{
				float scale = (float)(btnHeight - 10) / item_icon.height;
				DrawTextureEx(item_icon, { itemRect.x + 5, itemRect.y + 5 }, 0.0f, scale, WHITE);
			}

			if (is_item_hovered)
			{
				std::string label = other_items[i]->get_name();
				DrawRectangle(itemRect.x + 50, itemRect.y, 120, 30, DARKBLUE);
				DrawTextEx(cabin_sketch_font, label.c_str(), { itemRect.x + 50, itemRect.y }, 22, 1, WHITE);
			}
		}
	}
	else if (inv->food_tab == true)
	{
		for (int i = 0; i < food.size(); i++)
		{
			item_icon = *(food[i]->icon_texture);

			int row = i / 5;
			int col = i % 5;
			Rectangle itemRect = {
				window_center.x + (float)(startX + col * (btnWidth + right_padding)),
				window_center.y + (float)(startY + row * (btnHeight + bottom_padding)),
				(float)btnWidth,
				(float)btnHeight
			};

			bool is_item_hovered = CheckCollisionPointRec(mouse_pos, itemRect);

			if (is_item_hovered && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
			{
				food[i]->use(&p);
				p.sort_bag();
				break;
			}

			if (item_icon.id > 0)
			{
				float scale = (float)(btnHeight - 10) / item_icon.height;
				DrawTextureEx(item_icon, { itemRect.x + 5, itemRect.y + 5 }, 0.0f, scale, WHITE);
			}

			if (is_item_hovered)
			{
				std::string label = food[i]->get_name();
				DrawRectangle(itemRect.x + 50, itemRect.y, 120, 30, DARKBLUE);
				DrawTextEx(cabin_sketch_font, label.c_str(), { itemRect.x + 50, itemRect.y }, 22, 1, WHITE);
			}
		}
	}
	else if (inv->books_tab == true)
	{
		for (int i = 0; i < books.size(); i++)
		{
			item_icon = *(books[i]->icon_texture);

			int row = i / 5;
			int col = i % 5;
			Rectangle itemRect = {
				window_center.x + (float)(startX + col * (btnWidth + right_padding)),
				window_center.y + (float)(startY + row * (btnHeight + bottom_padding)),
				(float)btnWidth,
				(float)btnHeight
			};

			bool is_item_hovered = CheckCollisionPointRec(mouse_pos, itemRect);

			if (is_item_hovered && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
			{
				books[i]->use(&p);
				p.sort_bag();
				break;
			}

			if (item_icon.id > 0)
			{
				float scale = (float)(btnHeight - 10) / item_icon.height;
				DrawTextureEx(item_icon, { itemRect.x + 5, itemRect.y + 5 }, 0.0f, scale, WHITE);
			}

			if (is_item_hovered)
			{
				std::string label = books[i]->get_name();
				DrawRectangle(itemRect.x + 50, itemRect.y, 120, 30, DARKBLUE);
				DrawTextEx(cabin_sketch_font, label.c_str(), { itemRect.x + 50, itemRect.y }, 22, 1, WHITE);
			}
		}
	}
}


bool draw_drop(exploration* exp, chest* current_chest, bool& is_open)
{

	int window_w = GAME_WIDTH / 2;
	int window_h = GAME_HEIGHT / 2;
	int window_x_start = GAME_WIDTH / 2 - window_w / 2; 
	int windows_y_start = GAME_HEIGHT / 2 - window_h / 2;

	Texture2D item_icon;
	Vector2 mouse_pos = GetVirtualMousePosition();

	if (current_chest == nullptr || !is_open)
	{
		return false;
	}

	std::vector<item*>& target_loot = (!current_chest->enemy_loot.empty()) ? current_chest->enemy_loot : current_chest->chest_loot;

	if (target_loot.empty())
	{
		is_open = false;
		return true;
	}

	DrawRectangle(window_x_start, windows_y_start, window_w, window_h, Fade(BLACK, 0.85f));
	DrawRectangleLines(window_x_start, windows_y_start, window_w, window_h, RAYWHITE);

	Rectangle closeBtnRect = {
		(float)(window_x_start + window_w - 55),
		(float)(windows_y_start + 15),
		40.0f,
		40.0f
	};
	bool is_close_hovered = CheckCollisionPointRec(mouse_pos, closeBtnRect);

	DrawRectangleRec(closeBtnRect, is_close_hovered ? RED : MAROON);
	DrawRectangleLinesEx(closeBtnRect, 1, RAYWHITE);
	DrawTextEx(cabin_sketch_font, "X", { closeBtnRect.x + 12, closeBtnRect.y + 5 }, 28, 1, WHITE);

	if (is_close_hovered && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
	{
		is_open = false;
		return true;
	}

	Rectangle takeAllRect = {
		(float)(window_x_start + (window_w - 210) / 2),
		(float)(windows_y_start + window_h - 100),
		200.0f,
		80.0f
	};
	bool is_take_all_hovered = CheckCollisionPointRec(mouse_pos, takeAllRect);

	DrawRectangleRec(takeAllRect, is_take_all_hovered ? DARKGRAY : DARKGREEN);
	DrawRectangleLinesEx(takeAllRect, 1, RAYWHITE);
	DrawTextEx(cabin_sketch_font, "Take all", { takeAllRect.x + 12, takeAllRect.y + 20 }, 40, 1, WHITE);

	if (is_take_all_hovered && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
	{
		for (auto* it : target_loot)
		{
			if (it != nullptr)
			{
				exp->bohater.bag->add_item(it);
			}
		}
		target_loot.clear();
		exp->bohater.sort_bag();
		is_open = false;
		return true;
	}

	int startX = window_x_start + 60;
	int startY = windows_y_start + 80;
	int btnWidth = 60;
	int btnHeight = 60;
	int padding = 25;

	item* clicked_item = nullptr;

	for (int i = 0; i < target_loot.size(); i++)
	{
		if (target_loot[i] == nullptr)
		{
			continue;
		}

		item_icon = *target_loot[i]->icon_texture;

		int row = i / 6;
		int col = i % 6;

		Rectangle itemRect = {
			(float)(startX + col * (btnWidth + padding)),
			(float)(startY + row * (btnHeight + padding)),
			(float)btnWidth,
			(float)btnHeight
		};

		bool is_item_hovered = CheckCollisionPointRec(mouse_pos, itemRect);

		DrawRectangleRec(itemRect, is_item_hovered ? GRAY : DARKGRAY);
		DrawRectangleLinesEx(itemRect, 1, RAYWHITE);

		if (is_item_hovered && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
		{
			clicked_item = target_loot[i];
			break;
		}

		if (item_icon.id > 0)
		{
			float scale = (float)(btnHeight - 10) / item_icon.height;
			DrawTextureEx(item_icon, { itemRect.x + 5, itemRect.y + 5 }, 0.0f, scale, WHITE);
		}

		if (is_item_hovered)
		{
			std::string label = target_loot[i]->get_name();
			DrawRectangle(itemRect.x, itemRect.y - 45, 140, 35, DARKBLUE);
			DrawRectangleLines(itemRect.x, itemRect.y - 45, 140, 35, WHITE);
			DrawTextEx(cabin_sketch_font, label.c_str(), { itemRect.x + 5, itemRect.y - 40 }, 20, 1, WHITE);
		}
	}

	if (clicked_item != nullptr)
	{
		exp->bohater.take_item(current_chest, clicked_item);

		if (target_loot.empty())
		{
			is_open = false;
			return true;
		}
	}

	return false;
}



void draw_commentary()
{
	if (!gamestate::gameLogs.empty())
	{
		float przesuniecie_y = 0.0;
		for (const auto& log : gamestate::gameLogs | std::views::reverse | std::views::take(3))
		{
			DrawTextEx(cabin_sketch_font, log.c_str(), { 50, 410 + przesuniecie_y }, 32, 2, WHITE);
			przesuniecie_y += 20.0;
		}		
	}
}

void draw_map_tile(exploration* exp, int x, int y, int start_x, int start_y, int tile)
{
	int offset_x = start_x + (tile * x);
	int offset_y = start_y + (tile * y);

	DrawRectangle(offset_x, offset_y, tile, tile, DARKGREEN);
}

void draw_dungeon_map(exploration* exp, float player_x, float player_y)
{
	if (exp == nullptr || exp->world_map.empty())
	{
		return;
	}

	DrawRectangle(100, 100, 1080, 520, Fade(BLACK, 0.8));
	DrawRectangleLines(100, 100, 1080, 520, RAYWHITE);

	int tile = 10;
	int vision_range = 20;

	float exact_player_x_pos = (player_x) / 2.0f;
	float exact_player_y_pos = (player_y) / 2.0f;

	int player_x_pos_int = (int)std::floor(exact_player_x_pos);
	int player_y_pos_int = (int)std::floor(exact_player_y_pos);

	int center_box_x = 640;
	int center_box_y = 360;

	int start_x = center_box_x - (exact_player_x_pos * tile);
	int start_y = center_box_y - (exact_player_y_pos * tile);


	int min_x = std::max(0, player_x_pos_int - vision_range);
	int max_x = std::min(exp->szerokosc, player_x_pos_int + vision_range);

	int min_y = std::max(0, player_y_pos_int - vision_range);
	int max_y = std::min(exp->dlugosc, player_y_pos_int + vision_range);

	for (int x = min_x; x <= max_x; x++)
	{
		for (int y = min_y; y <= max_y; y++)
		{
			if (exp->dungeon[x][y] == 1)
			{
				draw_map_tile(exp, x, y, start_x, start_y, tile);
			}
		}
	}

	DrawCircle(center_box_x, center_box_y, 4.0f, YELLOW);
}



void draw_buttons(exploration* e) 
{

	DrawRectangle(GAME_WIDTH * 0.8 , GAME_HEIGHT * 0.9, 50, 20, WHITE);
	DrawText("I", GAME_WIDTH * 0.8 + 20, GAME_HEIGHT * 0.9, 20, DARKBROWN);

	DrawRectangle(GAME_WIDTH * 0.9, GAME_HEIGHT * 0.9, 50, 20, WHITE);
	DrawText("M", GAME_WIDTH * 0.9 + 20, GAME_HEIGHT * 0.9, 20, DARKBROWN);
}


	



		

void gamestate::draw_menu()
{
	if (IsKeyPressed(KEY_ESCAPE))
	{
		showMenu = !showMenu;
	}

	if (!showMenu) return;

	int windowWidth = GAME_WIDTH / 3.5f; 
	int windowHeight = GAME_HEIGHT / 2.0f;
	int windowX = (GAME_WIDTH - windowWidth) / 2;
	int windowY = (GAME_HEIGHT - windowHeight) / 2;

	DrawRectangle(windowX, windowY, windowWidth, windowHeight, Fade(BLACK, 0.85f));
	DrawRectangleLines(windowX, windowY, windowWidth, windowHeight, RAYWHITE);

	Vector2 mouse_pos = GetVirtualMousePosition();

	int btnWidth = windowWidth * 0.75f;
	int btnHeight = 60;
	int btnX = windowX + (windowWidth - btnWidth) / 2;

	Rectangle loadBtnRect = { (float)btnX, (float)(windowY + windowHeight * 0.22f), (float)btnWidth, (float)btnHeight };
	bool is_load_hovered = CheckCollisionPointRec(mouse_pos, loadBtnRect);

	DrawRectangleRec(loadBtnRect, is_load_hovered ? GRAY : DARKGRAY);
	DrawRectangleLinesEx(loadBtnRect, 1, RAYWHITE);

	Vector2 loadTextSize = MeasureTextEx(cabin_sketch_font, "Wczytaj Gre", 26, 1);
	DrawTextEx(cabin_sketch_font, "Wczytaj Gre", { loadBtnRect.x + (btnWidth - loadTextSize.x) / 2, loadBtnRect.y + (btnHeight - loadTextSize.y) / 2 }, 26, 1, WHITE);

	if (is_load_hovered && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
	{
		load_game();
	}

	Rectangle saveBtnRect = { (float)btnX, (float)(windowY + windowHeight * 0.42f), (float)btnWidth, (float)btnHeight };
	Vector2 saveTextSize = MeasureTextEx(cabin_sketch_font, "Zapisz Gre", 26, 1);

	if (can_save_game)
	{
		bool is_save_hovered = CheckCollisionPointRec(mouse_pos, saveBtnRect);

		DrawRectangleRec(saveBtnRect, is_save_hovered ? GRAY : DARKGRAY);
		DrawRectangleLinesEx(saveBtnRect, 1, RAYWHITE);
		DrawTextEx(cabin_sketch_font, "Zapisz Gre", { saveBtnRect.x + (btnWidth - saveTextSize.x) / 2, saveBtnRect.y + (btnHeight - saveTextSize.y) / 2 }, 26, 1, WHITE);

		if (is_save_hovered && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
		{
			save_game();
		}
	}
	else
	{
		DrawRectangleRec(saveBtnRect, Fade(BLACK, 0.6f));
		DrawRectangleLinesEx(saveBtnRect, 1, GRAY);
		DrawTextEx(cabin_sketch_font, "Zapisz Gre", { saveBtnRect.x + (btnWidth - saveTextSize.x) / 2, saveBtnRect.y + (btnHeight - saveTextSize.y) / 2 }, 26, 1, GRAY);
	}

	Rectangle closeBtnRect = { (float)btnX, (float)(windowY + windowHeight * 0.62f), (float)btnWidth, (float)btnHeight };
	bool is_close_hovered = CheckCollisionPointRec(mouse_pos, closeBtnRect);

	DrawRectangleRec(closeBtnRect, is_close_hovered ? GRAY : DARKGRAY);
	DrawRectangleLinesEx(closeBtnRect, 1, RAYWHITE);

	Vector2 closeTextSize = MeasureTextEx(cabin_sketch_font, "Powrot", 26, 1);
	DrawTextEx(cabin_sketch_font, "Powrot", { closeBtnRect.x + (btnWidth - closeTextSize.x) / 2, closeBtnRect.y + (btnHeight - closeTextSize.y) / 2 }, 26, 1, WHITE);

	if (is_close_hovered && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
	{
		showMenu = false;
	}
}

void DrawGlobalAnimation()
{
	if (!global_fx.is_playing)
	{
		return;
	}

	float scale = 4.0f;
	float frameWidth = (float)global_fx.texture.width / global_fx.frame_count;
	float frameHeight = (float)global_fx.texture.height;

	global_fx.position = { 1920.0f * 0.5f, 1080.0f * 0.5f };

	Rectangle sourceRec = {
		global_fx.current_frame * frameWidth,
		0.0f,
		frameWidth,
		frameHeight
	};

	Rectangle destRec = {
		global_fx.position.x,
		global_fx.position.y,
		frameWidth * scale,
		frameHeight * scale
	};

	Vector2 origin = {
		(frameWidth * scale) * 0.5f,
		(frameHeight * scale) * 0.5f
	};

	DrawTexturePro(global_fx.texture, sourceRec, destRec, origin, 0.0f, WHITE);
}

	

void graphics_init()
{
	const int screenWidth = 1280;
	const int screenHeight = 720;
	InitWindow(screenWidth, screenHeight, "textRPG");
	SetExitKey(KEY_NULL);
	DisableCursor();
	SetTargetFPS(60);

	
}

void draw_game_over() 
{
	ClearBackground(BLACK);
	DrawText("GAME OVER", 500, 300, 50, RED);
	DrawText("Nacisnij ESC aby wyjsc", 520, 370, 20, GRAY);
}




