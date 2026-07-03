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

void DrawExploration(exploration* exp)
{
	for (int i=1; i<=3; i++)
	{
		if (exp->world_map[i].enemy != nullptr)
		{
			auto* current_enemy = exp->world_map[i].enemy;

			if (!current_enemy->is_dead())
			{
				if (current_enemy->get_name() == "Goblin")
				{
					DrawBillboard(exp->camera, textures.goblin, current_enemy->get_position(), 2.0f, WHITE);
				}
				else if (current_enemy->get_name() == "Szkielet")
				{
					DrawBillboard(exp->camera, textures.skeleton, current_enemy->get_position(), 2.0f, WHITE);
				}
			}
		}
	}

	for (int cx = 0; cx < exp->szerokosc; cx++)
	{
		for (int cy = 0; cy < exp->dlugosc; cy++)
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
					Vector3 p_dolna = Vector3{ pozycja_dolna.x - 2.0f, 0.0f, pozycja_dolna.z };
					Vector3 p_gorna = Vector3{ pozycja_gorna.x - 2.0f, 2.0f, pozycja_gorna.z };
					DrawModelEx(objects.wall_tile, p_dolna, Vector3{ 0.0f, 1.0f, 0.0f }, 90.0f, Vector3{ 1.0f, 1.0f, 1.0f }, WHITE);
					DrawModelEx(objects.wall_tile, p_gorna, Vector3{ 0.0f, 1.0f, 0.0f }, 90.0f, Vector3{ 1.0f, 1.0f, 1.0f }, WHITE);
				
				}
				//ŒCIANA ZACHODNIA
				if ( cx < exp->szerokosc - 1 && exp->dungeon[cx + 1][cy] == 1)
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


void draw_game_scene(exploration* exp)
{

	
	ClearBackground(BLACK);

	BeginMode3D(exp->camera);
	DrawExploration(exp); 
	EndMode3D();


	

	DrawTextEx(arial_font, TextFormat("HP Bohatera: %d", exp->bohater.get_health()), { 70, 620 }, 20, 2, MAROON);
	DrawRectangle(70, 650, 300, 30, DARKGRAY);
	float hp_width = ((float)exp->bohater.get_health() / exp->bohater.get_max_health()) * 300;
	DrawRectangle(70, 650, (int)hp_width, 30, RED);

	DrawTextEx(arial_font, TextFormat("LvL %d", exp->bohater.get_level()), { 10, 530 }, 20, 2, WHITE);
	DrawTextEx(arial_font, TextFormat("XP: %d/%d", exp->bohater.get_xp(), exp->bohater.get_xp_to_level_up()), { 10, 500 }, 20, 2, WHITE);
	DrawRectangle(10, 600, 30, 100, DARKBLUE);
	float xp_height = (float)exp->bohater.get_xp() / exp->bohater.get_xp_to_level_up() * 100;
	DrawRectangle(10, 700-xp_height, 30, xp_height, WHITE);
}


void draw_chest_drop(exploration* exp)
{
	chest_drop* chest = dynamic_cast<chest_drop*>(exp->world_map[exp->current_node_id].current_event);
	if (chest != nullptr) 
	{
		Texture2D chest_texture = textures.chest_t;
		std::vector<item*>& chest_loot = chest->chest_ptr->chest_loot;
		DrawTextureEx(chest_texture, { 500, 260 }, 0, 0.10, WHITE);
		DrawText("ZNALAZLES  SKRZYNIE!", 400, 200, 30, GOLD);
		
		if (chest->is_chest_open == true)
		{
			if (chest_loot.empty())
			{
				chest->is_chest_open = false;
				chest->discard_chest();
				return;
			}
			else
			{
				DrawRectangle(100, 100, 1080, 520, Fade(BLACK, 0.8));
				DrawRectangleLines(100, 100, 1080, 520, RAYWHITE);
				int startX = 140;
				int startY = 180;
				int btnWidth = 180;
				int btnHeight = 60;
				int padding = 15;
				if (GuiButton({ 520,550,200,60 }, "Wez wszystko")) {
					chest->collect_loot();
					chest->discard_chest();	

					exp->bohater.sort_bag();
					return;
				}
				if (GuiButton({ 1000,100,50,50 }, "X")) {
					chest->is_chest_open = false;	
				}
				for (int i = 0; i < chest_loot.size(); i++) 
				{

					int row = i / 5;
					int col = i % 5;
					Rectangle itemRect = {
						(float)(startX + col * (btnWidth + padding)),
						(float)(startY + row * (btnHeight + padding)),
						(float)btnWidth,
						(float)btnHeight
					};


					std::string label = chest_loot[i]->get_name();

					if (GuiButton(itemRect, label.c_str())) {
						exp->bohater.take_item(chest->chest_ptr, chest_loot[i]);
						if (chest_loot.empty()) {
							chest->is_chest_open = false;
							chest->discard_chest();
							return;
						}
					}

				}
			}
		}
			else
			{
				if (GuiButton({ 400, 300, 200, 50 }, "Otworz")) 
				{
					chest->is_chest_open = true;
				}
			}

		
			
	}
		
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

void draw_map(map_state* map, exploration* exp)
{
	Node* current_Node = exp->get_current_node();

	DrawRectangle(100, 100, 1080, 520, Fade(BLACK, 0.8));
	DrawRectangleLines(100, 100, 1080, 520, RAYWHITE);
		
		float posX = current_Node->positionX;
		float posY = current_Node->positionY;

		auto i = exp->world_map.begin();

		while (i != exp->world_map.end())
		{			
			float posX = i->second.positionX;
			float posY = i->second.positionY;
			if (i->first != -1)
			{
				if(i->second.discovered)
				{
					DrawRectangle(posX, posY, 60, 60, DARKGREEN);
					DrawRectangleLinesEx({ posX, posY, 60, 60 }, 3, RAYWHITE);
				}
				if (i->first == exp->current_node_id)
				{
					DrawRectangle(posX, posY, 60, 60, GREEN);
					DrawRectangleLinesEx({ posX, posY, 60, 60 }, 3, RAYWHITE);
				}
				if (i->second.previous_id!=-1 && i->second.discovered)
				{
					Node& parent = exp->world_map[i->second.previous_id];
					float parent_posX = parent.positionX;
					float parent_posY = parent.positionY;
					DrawLineEx({ posX + 30, posY + 30 }, { parent_posX + 30, parent_posY + 30 }, 5, WHITE);
				}
			}

			i++;
		}
		
		if (GuiButton({ 1000,100,100,150 }, "Powrot")) {
			map->back_requested = true;
		}
}



void draw_buttons(exploration* e) 
{
	GuiSetFont(arial_font);
	GuiSetStyle(DEFAULT, TEXT_SIZE, 24);
	Node* current = e->get_current_node();
	
	if (GuiButton({ 1000, 350, 280, 150 }, "EKWIPUNEK")) {
		e->showInventory = true;
	}
	if (GuiButton({ 1000, 550, 280, 150 }, "MAPA")) {
		e->showMap = true;
	}
	Rectangle btnLeft = { 400, 520, 200, 70 };
	Rectangle btnRight = { 750, 520, 200, 70 };
	Rectangle btnBack = { 90, 520, 200, 70 };
	

	if (current->left_id != -1) {
		if (GuiButton(btnLeft, "Lewo")) {
			e->move_left();
		}
	}

	if (current->right_id != -1) {
		if (GuiButton(btnRight, "Prawo")) {
			e->move_right();
		}
	}

	if (current->previous_id != -1) {
		if (GuiButton(btnBack, "Powrot")) {
			e->move_back();
		}
	}
	
}
void draw_inventory_ui(player& p, inventory_state* inv) 
{
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
			int row = i / 5;
			int col = i % 5;
			Rectangle itemRect = {
				(float)(startX + col * (btnWidth + padding)),
				(float)(startY + row * (btnHeight + padding)),
				(float)btnWidth,
				(float)btnHeight
			};

			std::string label = equipment[i]->get_name();
			if (equipment[i]->is_equipped())
			{
				label = "[E] " + label;
			}


			if (GuiButton(itemRect, label.c_str()))
			{				
					equipment[i]->use(&p, -1);
			}
		}
	}
	else if (inv->usables_tab == true)
	{
		int itemToUse = -1; 

		for (int i = 0; i < usables.size(); i++)
		{
			int row = i / 5;
			int col = i % 5;
			Rectangle itemRect = {
				(float)(startX + col * (btnWidth + padding)),
				(float)(startY + row * (btnHeight + padding)),
				(float)btnWidth,
				(float)btnHeight
			};

			std::string label = usables[i]->get_name();
			if (GuiButton(itemRect, label.c_str()))
			{
				itemToUse = i;
				
			}
			
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
			int row = i / 5;
			int col = i % 5;
			Rectangle itemRect = {
				(float)(startX + col * (btnWidth + padding)),
				(float)(startY + row * (btnHeight + padding)),
				(float)btnWidth,
				(float)btnHeight
			};

			std::string label = scrolls[i]->get_name();
			if (GuiButton(itemRect, label.c_str()))
			{
				itemToUse = i;
			}

		}
		if (itemToUse != -1)
		{
			scrolls[itemToUse]->use(&p, -1);
			p.sort_bag();
			return;
		}

	}

	
	if (GuiButton({ 1000,100,100,100 }, "X"))
	{
		inv->equipment_tab = false;
		inv->usables_tab = false;
		inv->scrolls_tab = false;
		inv->request_back();
	}
}

	




void draw_battle_ui(battle* fight) {

	std::string e = fight->get_enemy_name();
	float hp_width = (float)fight->get_enemy_hp() / fight->get_enemy_max_hp() * 300;
	DrawRectangle(600, 650, 300, 30, DARKGRAY);
	DrawRectangle(600, 650, (int)hp_width, 30, RED);
	DrawTextEx(arial_font, TextFormat("HP Wroga: %d", fight->get_enemy_hp()), { 600, 620 }, 20, 2, MAROON);
	DrawRectangle(0, 400, 1000, 80, Color{ 245, 245, 220, 255 });
	DrawRectangleLinesEx({ 0, 400, 1000, 80 }, 5, Color{ 133, 94, 66, 255 });
		Texture2D enemy_texture;

		float scale = 0.17;
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
		
		if (!fight->showInventory) {
			
			if (GuiButton({ 90, 500, 250, 50 }, "SZYBKI ATAK")) {
				
				if (fight->player_cooldown <= 0)
				{
					fight->attack_clicked = true;
					fight->player_cooldown = 2.5;
				}
					
			}
			if (GuiButton({ 1000, 350, 280, 150 }, "EKWIPUNEK")) {
				fight->showInventory = true;
			}
			if (GuiButton({ 1000, 550, 280, 150 }, "MAPA")) {
				fight->showMap = true;
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



