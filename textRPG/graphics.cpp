#include <vector>
#include <algorithm>
#include "raylib.h"
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"
#include "graphics.h"
#include "character.h"
#include "inventory_class.h"
#include "textureManager.h"
#include <ranges>

extern Font arial_font;
extern Font pogrubione_arial_font;
float screen_width = 0.0f;
float screen_height = 0.0f;

void draw_login_screen(const std::string& current_name, bool has_error)
{
	ClearBackground(BLACK);

	DrawText( "LOGOWANIE", 500, 100, 50, GOLD);
	DrawTextEx(arial_font, "Wpisz swoj nickname (3-12 znakow, bez znakow specjalnych):", { 350, 180 }, 20, 2, GRAY);

	DrawRectangleLines(500, 230, 300, 40, LIGHTGRAY);
	DrawTextEx(arial_font, current_name.c_str(), { 510, 240 }, 20, 3, RAYWHITE);

	DrawTextEx(arial_font, "Wcisnij [ENTER] aby zatwierdzic", { 500, 300 }, 16, 2, DARKGRAY);

	if (has_error)
	{
		DrawTextEx(arial_font, "BLAD: Nick zawiera znaki specjalne lub ma zla dlugosc!", { 250, 350 }, 18, 2, RED);
	}
}

struct RenderObject
{
	float distance_sqr = 0.0f;

	chest* chest_ptr = nullptr;
	enemy* enemy_ptr = nullptr;
};

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
		// A. Jeœli to SKRZYNKA:
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
				DrawCube(c->position, 0.6f, 0.6f, 0.6f, BROWN);
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
				Vector3 draw_pos = { pos.x, pos.y + 0.4f, pos.z };
				DrawBillboard(exp->camera, textures.skeleton, draw_pos, 3.0f, WHITE);
				break;
			}
			case 2:
			{
				Vector3 draw_pos = { pos.x, pos.y, pos.z };
				DrawBillboard(exp->camera, textures.goblin, draw_pos, 2.0f, WHITE);
				break;
			}
			case 3:
			{
				Vector3 draw_pos = { pos.x, pos.y + 0.63f, pos.z };
				DrawBillboard(exp->camera, textures.troll, draw_pos, 3.5f, WHITE);
				break;
			}
			case 4:
			{
				Vector3 draw_pos = { pos.x, pos.y + 0.3f, pos.z };
				DrawBillboard(exp->camera, textures.bandits, draw_pos, 2.8f, WHITE);
				break;
			}
			case 5:
			{
				Vector3 draw_pos = { pos.x, pos.y + 0.45f, pos.z };
				DrawBillboard(exp->camera, textures.guard, draw_pos, 3.0f, WHITE);
				break;
			}
			case 6:
			{
				Vector3 draw_pos = { pos.x, pos.y + 0.55f, pos.z };
				DrawBillboard(exp->camera, textures.dragon, draw_pos, 3.5f, WHITE);
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

void DrawHUD(exploration* exp)
{
	DrawTextEx(arial_font, TextFormat("HP: %d", exp->bohater.get_health()), { 70, 620 }, 20, 2, MAROON);
	DrawRectangle(70, 650, 300, 30, DARKGRAY);
	float hp_width = ((float)exp->bohater.get_health() / exp->bohater.get_max_health()) * 300;
	DrawRectangle(70, 650, (int)hp_width, 30, RED);

	DrawTextEx(arial_font, TextFormat("LvL %d", exp->bohater.get_level()), { 10, 530 }, 20, 2, WHITE);
	DrawTextEx(arial_font, TextFormat("XP: %d/%d", exp->bohater.get_xp(), exp->bohater.get_xp_to_level_up()), { 10, 500 }, 20, 2, WHITE);
	DrawRectangle(10, 600, 30, 100, DARKBLUE);
	float xp_height = (float)exp->bohater.get_xp() / exp->bohater.get_xp_to_level_up() * 100;
	DrawRectangle(10, 700 - xp_height, 30, xp_height, WHITE);
}


void draw_game_scene(exploration* exp)
{

	
	ClearBackground(BLACK);

	BeginMode3D(exp->camera);
	DrawExploration(exp); 
	EndMode3D();
	DrawHUD(exp);


}


bool draw_drop(exploration* exp, chest* current_chest, bool& is_open)
{
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
	
	DrawRectangle(100, 100, 1080, 520, Fade(BLACK, 0.8));
	DrawRectangleLines(100, 100, 1080, 520, RAYWHITE);

	int startX = 140;
	int startY = 180;
	int btnWidth = 180;
	int btnHeight = 60;
	int padding = 15;

	if (GuiButton({ 520,550,200,60 }, "Wez wszystko")) {
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

	if (GuiButton({ 1000,100,50,50 }, "X")) {
		is_open = false;
		return true;
	}

	item* clicked_item = nullptr;

	for (int i = 0; i < target_loot.size(); i++)
	{
		if (target_loot[i] == nullptr)
		{
			continue;
		}
		int row = i / 5;
		int col = i % 5;
		Rectangle itemRect = {
			(float)(startX + col * (btnWidth + padding)),
			(float)(startY + row * (btnHeight + padding)),
			(float)btnWidth,
			(float)btnHeight
		};

		std::string label = target_loot[i]->get_name();

		if (GuiButton(itemRect, label.c_str())) 
		{
			clicked_item = target_loot[i];
			break;
		}
	}
	if (clicked_item != nullptr)
	{
		exp->bohater.take_item(current_chest, clicked_item);

		if (target_loot.empty()) {
			is_open = false;
			return true;
		}
	}
	
	return false;
}



//RANGES
void draw_commentary()
{
	if (!gamestate::gameLogs.empty())
	{
		float przesuniecie_y = 0.0;
		for (const auto& log : gamestate::gameLogs | std::views::reverse | std::views::take(3))
		{
			DrawTextEx(arial_font, log.c_str(), { 50, 410 + przesuniecie_y }, 25, 2, PINK);
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

	float exact_player_x_pos = (player_x) / 2.0f + 1.0f;
	float exact_player_y_pos = (player_y) / 2.0f;

	int player_x_pos_int = (int)std::floor(exact_player_x_pos);
	int player_y_pos_int = (int)std::floor(exact_player_y_pos);

	//Srodek ramki
	int center_box_x = 640;
	int center_box_y = 360;

	int start_x = center_box_x - (exact_player_x_pos * tile);
	int start_y = center_box_y - (exact_player_y_pos * tile);


	int min_x = std::max(0, player_x_pos_int - vision_range);
	int max_x = std::min(exp->szerokosc - 1, player_x_pos_int + vision_range);

	int min_y = std::max(0, player_y_pos_int - vision_range);
	int max_y = std::min(exp->dlugosc - 1, player_y_pos_int + vision_range);

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


	screen_width = GetScreenWidth();
	screen_height = GetScreenHeight();


	DrawRectangle(screen_width * 0.8 , screen_height * 0.9, 50, 20, WHITE);
	DrawText("I", screen_width * 0.8 + 20, screen_height * 0.9, 20, DARKBROWN);

	DrawRectangle(screen_width * 0.9, screen_height * 0.9, 50, 20, WHITE);
	DrawText("M", screen_width * 0.9 + 20, screen_height * 0.9, 20, DARKBROWN);


}
void draw_inventory_ui(player& p, inventory_state* inv) 
{
	Texture2D item_icon;

	p.sort_bag();
	DrawRectangle(100, 100, 1080, 520, Fade(BLACK, 0.8));
	DrawRectangleLines(100, 100, 1080, 520, RAYWHITE);

	
	auto& items = p.bag->items;

	auto& equipment = p.equipment->items;
	auto& usables = p.usables->items;
	auto& scrolls = p.scrolls->items;

	int startX = 140;
	int startY = 400;
	int btnWidth = 180;
	int btnHeight = 60;
	int padding = 15;


	if (GuiButton({ 200, 180, 150, 80 }, "Equipment"))
	{
		inv->equipment_tab = true;
		inv->usables_tab = false;
		inv->scrolls_tab = false;

	}
	if (GuiButton({ 500, 180, 150, 80 }, "Usables"))
	{
		inv->usables_tab = true;
		inv->equipment_tab = false;
		inv->scrolls_tab = false;
	}
	if (GuiButton({ 800, 180, 150, 80 }, "Scrolls"))
	{
		inv->usables_tab = false;
		inv->equipment_tab = false;
		inv->scrolls_tab = true;
	}
	
	if (inv->equipment_tab == true)
	{
		for (int i = 0; i < equipment.size(); i++)
		{
			item_icon = *(equipment[i]->icon_texture);

			int row = i / 5;
			int col = i % 5;
			Rectangle itemRect = {
				(float)(startX + col * (btnWidth + padding)),
				(float)(startY + row * (btnHeight + padding)),
				(float)btnWidth,
				(float)btnHeight
			};		
			std::string label = equipment[i]->get_name();
			
			
			if (GuiButton(itemRect,""))
			{				
					equipment[i]->use(&p, -1);
			}
			if (item_icon.id > 0)
			{
				float scale = (float)(btnHeight - 10) / item_icon.height;
				DrawTextureEx(item_icon, { itemRect.x+5, itemRect.y+5 }, 0.0f, scale, WHITE);
			}
			if (equipment[i]->is_equipped())
			{
				DrawTextEx(arial_font, "[E]", { itemRect.x + 55, itemRect.y + 35 }, 18, 1, WHITE);
			}
			DrawTextEx(arial_font, label.c_str(), { itemRect.x + 55, itemRect.y + 18 }, 18, 1, WHITE);
		}
	}
	else if (inv->usables_tab == true)
	{
		int itemToUse = -1; 

		for (int i = 0; i < usables.size(); i++)
		{
			item_icon = *(usables[i]->icon_texture);
			int row = i / 5;
			int col = i % 5;
			Rectangle itemRect = {
				(float)(startX + col * (btnWidth + padding)),
				(float)(startY + row * (btnHeight + padding)),
				(float)btnWidth,
				(float)btnHeight
			};

			std::string label = usables[i]->get_name();
			if (GuiButton(itemRect, ""))
			{
				itemToUse = i;
				
			}
			if (item_icon.id > 0)
			{
				float scale = (float)(btnHeight - 10) / item_icon.height;
				DrawTextureEx(item_icon, { itemRect.x + 5, itemRect.y + 5 }, 0.0f, scale, WHITE);
			}

			DrawTextEx(arial_font, label.c_str(), { itemRect.x + 55, itemRect.y + 18 }, 18, 1, WHITE);
		}
		if (itemToUse != -1)
		{
			usables[itemToUse]->use(&p, -1);
			p.sort_bag();
			return;
		}
		
	}
	else if (inv->scrolls_tab == true)
	{
		int itemToUse = -1;

		for (int i = 0; i < scrolls.size(); i++)
		{
			item_icon = *(scrolls[i]->icon_texture);
			int row = i / 5;
			int col = i % 5;
			Rectangle itemRect = {
				(float)(startX + col * (btnWidth + padding)),
				(float)(startY + row * (btnHeight + padding)),
				(float)btnWidth,
				(float)btnHeight
			};

			std::string label = scrolls[i]->get_name();
			if (GuiButton(itemRect,""))
			{
				itemToUse = i;
			}
			if (item_icon.id > 0)
			{
				float scale = (float)(btnHeight - 10) / item_icon.height;
				DrawTextureEx(item_icon, { itemRect.x + 5, itemRect.y + 5 }, 0.0f, scale, WHITE);
			}

			DrawTextEx(arial_font, label.c_str(), { itemRect.x + 55, itemRect.y + 18 }, 18, 1, WHITE);
		}
		if (itemToUse != -1)
		{
			scrolls[itemToUse]->use(&p, -1);
			p.sort_bag();
			return;
		}

	}
}

	

void draw_battle_ui(battle* fight)
{

	screen_width = GetScreenWidth();
	screen_height = GetScreenHeight();

	std::string e = fight->get_enemy_name();
	float p_hp_width = ((float)fight->p_ref.get_health() / fight->p_ref.get_max_health()) * 300;
	float e_hp_width = (float)fight->get_enemy_hp() / fight->get_enemy_max_hp() * 300;
	float xp_height = (float)fight->p_ref.get_xp() / fight->p_ref.get_xp_to_level_up() * 100;

	DrawRectangle(screen_width * 0.8, screen_height * 0.9, 50, 20, WHITE);
	DrawText("I", screen_width * 0.8 + 20, screen_height * 0.9, 20, DARKBROWN);
	//PASEK ZYCIA GRACZA
	DrawTextEx(arial_font, TextFormat("HP Bohatera: %d", fight->p_ref.get_health()), { 70, 620 }, 20, 2, MAROON);
	DrawRectangle(70, 650, 300, 30, DARKGRAY);
	DrawRectangle(70, 650, (int)p_hp_width, 30, RED);
	DrawTextEx(arial_font, TextFormat("LvL %d", fight->p_ref.get_level()), { 10, 530 }, 20, 2, WHITE);
	DrawTextEx(arial_font, TextFormat("XP: %d/%d", fight->p_ref.get_xp(), fight->p_ref.get_xp_to_level_up()), { 10, 500 }, 20, 2, WHITE);
	DrawRectangle(10, 600, 30, 100, DARKBLUE);
	DrawRectangle(10, 700 - xp_height, 30, xp_height, WHITE);

	//PASEK ZYCIA PRZECIWNIKA
	DrawRectangle(600, 650, 300, 30, DARKGRAY);
	DrawRectangle(600, 650, (int)e_hp_width, 30, RED);
	DrawTextEx(arial_font, TextFormat("HP Wroga: %d", fight->get_enemy_hp()), { 600, 620 }, 20, 2, MAROON);
	DrawRectangle(0, 400, 1000, 80, Color{ 245, 245, 220, 255 });
	DrawRectangleLinesEx({ 0, 400, 1000, 80 }, 5, Color{ 133, 94, 66, 255 });

	Texture2D enemy_texture;

	float scale = 0.20;
	if (e == "Szkielet") enemy_texture = textures.skeleton;
	else if (e == "Troll")  enemy_texture = textures.troll;
	else if (e == "Bandyci") enemy_texture = textures.bandits;
	else if (e == "Straznik") enemy_texture = textures.guard;
	else if (e == "Smok") enemy_texture = textures.dragon;
	else if (e == "Goblin")
	{
		enemy_texture = textures.goblin;
		scale = 0.47;
	}

	float przeskalowana_szerokosc_wroga = (float)enemy_texture.width * scale;
	float posX = 670.0 - (przeskalowana_szerokosc_wroga / 2.0);
	float posY = 55.0;

	float centerX = 430.0 + (((float)enemy_texture.width * 0.17) / 2.0);
	float centerY = 80.0 + (((float)enemy_texture.height * 0.17) / 2.0);
	if (e != "Goblin")
	{
		DrawCircleGradient({ centerX, centerY }, 300, Color{ 255, 0, 0, 60 }, BLANK);
	}
	else
	{
		DrawCircleGradient({ 650, 200 }, 200, Color{ 255, 0, 0, 60 }, BLANK);
	}

	DrawTextureEx(enemy_texture, { posX, posY }, 0, scale, WHITE);

	float szerokosc_tekstu_e = (float)MeasureText(e.c_str(), 24);
	float posX_text = 675 - (szerokosc_tekstu_e / 2);

	DrawTextEx(pogrubione_arial_font, e.c_str(), { posX_text, 320 }, 24, 2, RED);

	Rectangle battle_menu_button = { screen_width * 0.2, screen_height * 0.7, 120, 30 };
	int button_spacing = battle_menu_button.width + 10;

	/*DrawRectangleRec(battle_menu_button, WHITE);
	DrawTextEx(pogrubione_arial_font, "Attack", { battle_menu_button.x, battle_menu_button.y }, 20, 2, DARKBLUE);
	DrawRectangleRec({ battle_menu_button.x + button_spacing, battle_menu_button.y, battle_menu_button.width, battle_menu_button.height }, WHITE);
	DrawTextEx(pogrubione_arial_font, "Skills", { battle_menu_button.x + button_spacing, battle_menu_button.y }, 20, 2, DARKBLUE);
	DrawRectangleRec({battle_menu_button.x + button_spacing * 2, battle_menu_button.y, battle_menu_button.width, battle_menu_button.height}, WHITE);
	DrawTextEx(pogrubione_arial_font, "Guard", { battle_menu_button.x + button_spacing * 2, battle_menu_button.y }, 20, 2, DARKBLUE);
	DrawRectangleRec({ battle_menu_button.x + button_spacing * 3, battle_menu_button.y, battle_menu_button.width, battle_menu_button.height }, WHITE);
	DrawTextEx(pogrubione_arial_font, "Item", { battle_menu_button.x + button_spacing * 3, battle_menu_button.y }, 20, 2, DARKBLUE);*/
	
	if (GuiButton({ battle_menu_button }, "Attack")) {
				
		if (fight->player_cooldown <= 0)
		{
			fight->attack_clicked = true;
			fight->player_cooldown = 1.0;
		}
					
	}
	if (GuiButton({ battle_menu_button.x + button_spacing * 2, battle_menu_button.y, battle_menu_button.width, battle_menu_button.height }, "Guard")) 
	{
		if (fight->player_cooldown <= 0)
		{
			fight->guard_clicked = true;
			fight->player_cooldown = 1.0;
		}
		
	}
	
}

		


void gamestate::draw_menu()
{
	if (IsKeyPressed(KEY_ESCAPE))
	{ 
		showMenu = !showMenu;
	}

	if (showMenu)
	{
		DrawRectangle(350, 100, 580, 550, Fade(BLACK, 0.8));
		DrawRectangleLines(350, 100, 580, 550, LIGHTGRAY);
		if (!can_save_game)
		{
			GuiDisable();
		}
		if (GuiButton({ 550, 325, 200, 80 }, "Zapisz Gre")) {
			save_game();
		}
		if (GuiButton({ 550, 200, 200, 80 }, "Wczytaj Gre")) {
			load_game();
		}
		if (!can_save_game)
		{
			GuiEnable();
		}
	}
	
		
}

void DrawGlobalAnimation()
{
	if (!global_fx.is_playing)
	{
		return;
	}
	float frameWidth = (float)global_fx.texture.width / global_fx.frame_count;
	float frameHeight = (float)global_fx.texture.height;

	float scale = 2.5;

	Rectangle sourceRec = {
		global_fx.current_frame * frameWidth,
		0.0,
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
		(frameWidth * scale) / 2,
		(frameHeight * scale) / 2
	};

	DrawTexturePro(global_fx.texture, sourceRec, destRec, origin, 0.0, WHITE);
}

	

void graphics_init()
{
	const int screenWidth = 1280;
	const int screenHeight = 720;
	InitWindow(screenWidth, screenHeight, "textRPG");
	DisableCursor();
	SetTargetFPS(60);

	
}

void draw_game_over() 
{
	ClearBackground(BLACK);
	DrawText("GAME OVER", 500, 300, 50, RED);
	DrawText("Nacisnij ESC aby wyjsc", 520, 370, 20, GRAY);
}



